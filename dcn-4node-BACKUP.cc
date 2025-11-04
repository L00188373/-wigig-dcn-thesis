/*
 * Copyright (c) 2015-2020 IMDEA Networks Institute
 * Author: Hany Assasa <hany.assasa@gmail.com>
 */

#include "wigig-examples-common-functions.h"

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/wigig-module.h"
#include "ns3/flow-monitor-module.h"

#include <complex>
#include <iomanip>
#include <string>

/**
 * Simulation Objective:
 * Evaluate the maximum achievable throughput for each MCS defined in IEEE
 * 802.11ad.
 *
 * Network Topology:
 * The scenario consists of two DMG AdHoc STAs.
 *
 *          DMG STA [1] (0,0)                       DMG STA [2] (+1,0)
 *
 * Simulation Description:
 * The DMG STA[2] generates a UDP traffic towards the DMG STA [1].
 *
 * Running Simulation:
 * ./waf --run wigig-evaluate-achievable-throughput-example
 *
 * Simulation Output:
 * The simulation generates the following traces:
 * 1. Custom traces to report PHY and MAC layer statistics.
 */

NS_LOG_COMPONENT_DEFINE("EvaluateAchievableThroughput");

using namespace ns3;
using namespace std;

/**  Application Variables **/
Ptr<PacketSink> packetSink;
Ptr<OnOffApplication> onoff;

/* Network Nodes */
Ptr<Node> apWigigNode, staWigigNode;
Ptr<WigigNetDevice> staWigigNetDevice, apWigigNetDevice;
Ptr<WigigPhy> staWigigPhy, apWigigPhy;

void
SetAntennaConfigurations(NetDeviceContainer& apDevice, NetDeviceContainer& staDevice)
{
    Ptr<WigigNetDevice> apWigigNetDevice = DynamicCast<WigigNetDevice>(apDevice.Get(0));
    Ptr<WigigNetDevice> staWigigNetDevice = DynamicCast<WigigNetDevice>(staDevice.Get(0));
    Ptr<AdhocWigigMac> apWigigMac = DynamicCast<AdhocWigigMac>(apWigigNetDevice->GetMac());
    Ptr<AdhocWigigMac> staWigigMac = DynamicCast<AdhocWigigMac>(staWigigNetDevice->GetMac());
    apWigigMac->AddAntennaConfig(1, 1, staWigigMac->GetAddress());
    staWigigMac->AddAntennaConfig(5, 1, apWigigMac->GetAddress());
    apWigigMac->SteerAntennaToward(staWigigMac->GetAddress());
    staWigigMac->SteerAntennaToward(apWigigMac->GetAddress());
}

int
main(int argc, char* argv[])
{
    uint32_t payloadSize = 1472; /* Application payload size in bytes. */
    string msduAggSize = "max";  /* The maximum aggregation size for A-MSDU in Bytes. */
    string mpduAggSize = "max";  /* The maximum aggregation size for A-MPDU in Bytes. */
    string queueSize = "4000p";  /* Wigig MAC Queue Size. */
    uint32_t channel = 2;        /* WiGig channel number. */
    bool enableRts = false;      /* Flag to indicate if RTS/CTS handskahre is enabled
                                    or disabled. */
    uint32_t rtsThreshold = 0;   /* RTS/CTS handshake threshold. */
    double simulationTime = 1;   /* Simulation time in seconds per MCS. */
    string wigigErrorModel = "src/wigig/model/reference/ErrorModel/LookupTable_1458.txt";

    /* Command line argument parser setup. */
    CommandLine cmd;
    cmd.AddValue("payloadSize", "Application payload size in bytes", payloadSize);
    cmd.AddValue("msduAggSize", "The maximum aggregation size for A-MSDU in Bytes", msduAggSize);
    cmd.AddValue("mpduAggSize", "The maximum aggregation size for A-MPDU in Bytes", mpduAggSize);
    cmd.AddValue("enableRts", "Enable or disable RTS/CTS handshake", enableRts);
    cmd.AddValue("queueSize", "The maximum size of the Wigig MAC Queue", queueSize);
    cmd.AddValue("channel", "WiGig channel number", channel);
    cmd.AddValue("simulationTime", "Simulation time in Seconds per MCS", simulationTime);
    cmd.AddValue("errorModel", "Path to the Wigig error model", wigigErrorModel);
    cmd.Parse(argc, argv);

    AsciiTraceHelper ascii; /* ASCII Helper. */
    Ptr<OutputStreamWrapper> outputFile = ascii.CreateFileStream("AchievableThroughputTable.csv");
    *outputFile->GetStream() << "MCS,THROUGHPUT" << std::endl;

    /* Validate WiGig standard value */
    string wigigModePrefix = "DmgMcs";
    uint modes = 1;   /* The number of PHY modes we have. */
    uint maxMcs = 24; /* The maximum MCS index. */

    /* Validate A-MSDU and A-MPDU values */
    ValidateFrameAggregationAttributes(msduAggSize, mpduAggSize);
    /* Configure RTS/CTS and Fragmentation */
    ConfigureRtsCtsAndFragmenatation(enableRts, rtsThreshold);
    /* Wigig MAC Queue Parameters */
    ChangeQueueSize(queueSize);
 
    std::cout << "\n╔════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║   WiGig Data Center Network - 4 Rack Simulation       ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════╝\n" << std::endl;
    std::cout << "Testing wireless connections between server racks" << std::endl;
    std::cout << "Rack spacing: 1-2 meters (realistic data center)" << std::endl;
    std::cout << "Technology: 60 GHz WiGig\n" << std::endl;
    std::cout << "Running tests for all speed levels (MCS 1-24)...\n" << std::endl;


    for (uint mode = 1; mode <= modes; mode++)
    {
        for (uint mcs = 1; mcs <= maxMcs; mcs++)
        {
            WifiMode mode = WifiMode(wigigModePrefix + std::to_string(mcs));

            /* Get the nominal PHY rate and use it as the data of the OnOff
             * application */
            uint64_t dataRate = mode.GetPhyRate(2160);

            /**** WigigHelper is a meta-helper: it helps creates helpers ****/
            WigigHelper wigig;
            // wigig.EnableLogComponents ();

            /**** Set up Channel ****/
            WigigChannelHelper wigigChannel;
            /* Simple propagation delay model */
            wigigChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
            /* Friis model with standard-specific wavelength */
            wigigChannel.AddPropagationLoss("ns3::FriisPropagationLossModel",
                                            "Frequency",
                                            DoubleValue(60.48e9));

            /**** Setup physical layer ****/
            WigigPhyHelper wigigPhy(wigigErrorModel);
            /* Nodes will be added to the channel we set up earlier */
            wigigPhy.SetChannel(wigigChannel.Create());
            /* All nodes transmit at 0 dBm == 1 mW, no adaptation */
            wigigPhy.Set("TxPowerStart", DoubleValue(0.0));
            wigigPhy.Set("TxPowerEnd", DoubleValue(0.0));
            wigigPhy.Set("TxPowerLevels", UintegerValue(1));
            /* Set operating channel */
            wigigPhy.Set("ChannelNumber", UintegerValue(channel));
            /* Add support for the OFDM PHY */
            wigigPhy.Set("SupportOfdmPhy", BooleanValue(true));
            /* Set default algorithm for all nodes to be constant rate */
            wigig.SetRemoteStationManager("ns3::ConstantRateWigigManager",
                                          "DataMode",
                                          StringValue(wigigModePrefix + std::to_string(mcs)));

            /* Make two nodes and set them up with the PHY and the MAC */
            NodeContainer wigigNodes;
            wigigNodes.Create(4);
            apWigigNode = wigigNodes.Get(0);
            staWigigNode = wigigNodes.Get(1);

            /* Add a DMG upper mac */
            WigigMacHelper wigigMac;

            /* Set Analytical Codebook for the WiGig Devices */
            wigig.SetCodebook("ns3::CodebookAnalytical",
                              "CodebookType",
                              EnumValue(SIMPLE_CODEBOOK),
                              "Antennas",
                              UintegerValue(1),
                              "Sectors",
                              UintegerValue(8));

            /* Create Wigig Network Devices (WifiNetDevice) */
            wigigMac.SetType("ns3::AdhocWigigMac",
                             "BE_MaxAmpduSize",
                             StringValue(mpduAggSize),
                             "BE_MaxAmsduSize",
                             StringValue(msduAggSize));

            NetDeviceContainer apDevice;
            apDevice = wigig.Install(wigigPhy, wigigMac, apWigigNode);

            NetDeviceContainer staDevice;
            staDevice = wigig.Install(wigigPhy, wigigMac, staWigigNode);

            /* Set the best antenna configurations */
            Simulator::ScheduleNow(&SetAntennaConfigurations, apDevice, staDevice);

            /* Setting mobility model */
            MobilityHelper mobility;
            Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
            positionAlloc->Add(Vector(0.0, 0.0, 0.0)); /* WiGig PCP/AP */
            positionAlloc->Add(Vector(1.0, 0.0, 0.0)); /* WiGig STA */
            positionAlloc->Add(Vector(0.0, 2.0, 0.0)); /* Rack 2 */
            positionAlloc->Add(Vector(1.0, 2.0, 0.0)); /* Rack 3 */


            mobility.SetPositionAllocator(positionAlloc);
            mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
            mobility.Install(wigigNodes);

            /* Internet stack*/
            InternetStackHelper stack;
            stack.Install(wigigNodes);

            Ipv4AddressHelper address;
            address.SetBase("10.0.0.0", "255.255.255.0");
            Ipv4InterfaceContainer apInterface;
            apInterface = address.Assign(apDevice);
            Ipv4InterfaceContainer staInterface;
            staInterface = address.Assign(staDevice);

            std::cout << "\nRack IP Addresses:" << std::endl;
            std::cout << "  Rack 1 (AP):  " << apInterface.GetAddress(0) << std::endl;
            std::cout << "  Rack 2 (STA): " << staInterface.GetAddress(0) << std::endl;
            std::cout << std::endl;


            /* Populate routing table */
            Ipv4GlobalRoutingHelper::PopulateRoutingTables();

            /* We do not want any ARP packets */
            PopulateArpCache();

            /* Install Simple UDP Server on the WiGig PCP/AP */
            PacketSinkHelper sinkHelper("ns3::UdpSocketFactory",
                                        InetSocketAddress(Ipv4Address::GetAny(), 9999));
            ApplicationContainer sinkApp = sinkHelper.Install(apWigigNode);
            packetSink = StaticCast<PacketSink>(sinkApp.Get(0));
            sinkApp.Start(Seconds(0.0));

            /* Install Simple UDP Server on the WiGig STA for reverse traffic */
            PacketSinkHelper sinkHelperReverse("ns3::UdpSocketFactory",
                                        InetSocketAddress(Ipv4Address::GetAny(), 9998));
            ApplicationContainer sinkAppReverse = sinkHelperReverse.Install(staWigigNode);
            sinkAppReverse.Start(Seconds(0.0));


            /* Install UDP Transmitter on the WiGig STA */
            ApplicationContainer srcApp;
            OnOffHelper src("ns3::UdpSocketFactory",
                            InetSocketAddress(apInterface.GetAddress(0), 9999));
            src.SetAttribute("PacketSize", UintegerValue(payloadSize));
            src.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1e6]"));
            src.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
            src.SetAttribute("DataRate", DataRateValue(DataRate(dataRate)));
            srcApp = src.Install(staWigigNode);
            srcApp.Start(Seconds(0.0));
            srcApp.Stop(Seconds(simulationTime));
            onoff = StaticCast<OnOffApplication>(srcApp.Get(0));
           


            /* Install UDP Transmitter on the WiGig AP for reverse traffic */
            ApplicationContainer srcAppReverse;
            OnOffHelper srcReverse("ns3::UdpSocketFactory",
                            InetSocketAddress(staInterface.GetAddress(0), 9998));
            srcReverse.SetAttribute("PacketSize", UintegerValue(payloadSize));
            srcReverse.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1e6]"));
            srcReverse.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
            srcReverse.SetAttribute("DataRate", DataRateValue(DataRate(dataRate)));
            srcAppReverse = srcReverse.Install(apWigigNode);
            srcAppReverse.Start(Seconds(0.0));
            srcAppReverse.Stop(Seconds(simulationTime));


            Simulator::Stop(Seconds(simulationTime));
            FlowMonitorHelper flowmon;
            Ptr<FlowMonitor> monitor = flowmon.InstallAll();
            Simulator::Run();
            monitor->CheckForLostPackets();
            Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
            std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();
            
            std::cout << "\n=== Flow Statistics ===" << std::endl;
            for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin(); i != stats.end(); ++i)
            {
                double throughput = i->second.rxBytes * 8.0 / simulationTime / 1000000.0;
                double packetLoss = 0.0;
                if (i->second.txPackets > 0)
                    packetLoss = ((i->second.txPackets - i->second.rxPackets) * 100.0) / i->second.txPackets;
                double avgDelay = 0.0;
                if (i->second.rxPackets > 0)
                    avgDelay = i->second.delaySum.GetMilliSeconds() / i->second.rxPackets;
                
                Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
                std::cout << "Traffic from " << t.sourceAddress << " to " << t.destinationAddress << std::endl;
                std::cout << "  Throughput: " << throughput << " Mbps" << std::endl;
                std::cout << "  Packet Loss: " << packetLoss << " %" << std::endl;
                std::cout << "  Avg Delay: " << avgDelay << " ms" << std::endl;
            }
            
            Simulator::Destroy();

            *outputFile->GetStream()
                << mcs << "," << static_cast<double>(packetSink->GetTotalRx()) * 8.0 / 1e6
                << std::endl;
            std::cout << "MCS " << mcs << ": "
                      << static_cast<double>(packetSink->GetTotalRx()) * 8.0 / 1e6 << " Mbit/s"
                      << std::endl;
        }
    }

    return 0;
}
