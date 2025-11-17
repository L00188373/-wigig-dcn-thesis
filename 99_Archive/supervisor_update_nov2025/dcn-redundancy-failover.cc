/*
 * WiGig Link Redundancy with Failover Simulation
 * Simple 2-node test showing link blocking and recovery
 */

#include "wigig-examples-common-functions.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wigig-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

#include <iomanip>

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE("DcnRedundancyFailover");

Ptr<WigigPhy> phyA, phyB;

void SetAntennaConfigurations(NetDeviceContainer& dev1, NetDeviceContainer& dev2)
{
    Ptr<WigigNetDevice> device1 = DynamicCast<WigigNetDevice>(dev1.Get(0));
    Ptr<WigigNetDevice> device2 = DynamicCast<WigigNetDevice>(dev2.Get(0));
    Ptr<AdhocWigigMac> mac1 = DynamicCast<AdhocWigigMac>(device1->GetMac());
    Ptr<AdhocWigigMac> mac2 = DynamicCast<AdhocWigigMac>(device2->GetMac());
    
    mac1->AddAntennaConfig(1, 1, mac2->GetAddress());
    mac2->AddAntennaConfig(5, 1, mac1->GetAddress());
    mac1->SteerAntennaToward(mac2->GetAddress());
    mac2->SteerAntennaToward(mac1->GetAddress());
}

void BlockLink()
{
    std::cout << "\n[" << Simulator::Now().GetSeconds() << "s] OBSTRUCTION: Link blocked!" << std::endl;
    phyA->SetTxPowerStart(-50.0);
    phyA->SetTxPowerEnd(-50.0);
    phyB->SetTxPowerStart(-50.0);
    phyB->SetTxPowerEnd(-50.0);
}

void UnblockLink()
{
    std::cout << "[" << Simulator::Now().GetSeconds() << "s] RECOVERY: Link restored!" << std::endl;
    phyA->SetTxPowerStart(10.0);
    phyA->SetTxPowerEnd(10.0);
    phyB->SetTxPowerStart(10.0);
    phyB->SetTxPowerEnd(10.0);
}

int main(int argc, char *argv[])
{
    uint32_t payloadSize = 1472;
    string msduAggSize = "7935";
    string mpduAggSize = "262143";
    string queueSize = "4000p";
    uint32_t channel = 2;
    bool enableRts = false;
    uint32_t rtsThreshold = 0;
    uint32_t mcs = 12;
    double simulationTime = 15.0;
    double obstructionStart = 5.0;
    double obstructionEnd = 10.0;
    double distance = 1.82;
    
    string wigigErrorModel = "src/wigig/model/reference/ErrorModel/LookupTable_1458.txt";

    CommandLine cmd;
    cmd.AddValue("mcs", "WiGig MCS (1-24)", mcs);
    cmd.AddValue("simulationTime", "Simulation time", simulationTime);
    cmd.Parse(argc, argv);

    ValidateFrameAggregationAttributes(msduAggSize, mpduAggSize);
    ConfigureRtsCtsAndFragmenatation(enableRts, rtsThreshold);
    ChangeQueueSize(queueSize);

    std::cout << "\n======================================" << std::endl;
    std::cout << "  WiGig Redundancy Test" << std::endl;
    std::cout << "======================================\n" << std::endl;
    std::cout << "MCS: " << mcs << std::endl;
    std::cout << "Distance: " << distance << "m\n" << std::endl;
    std::cout << "Timeline:" << std::endl;
    std::cout << "  0-" << obstructionStart << "s:   Normal" << std::endl;
    std::cout << "  " << obstructionStart << "-" << obstructionEnd << "s:  Blocked" << std::endl;
    std::cout << "  " << obstructionEnd << "-" << simulationTime << "s: Restored\n" << std::endl;

    // Get PHY rate
    string wigigModePrefix = "DmgMcs";
    WifiMode mode = WifiMode(wigigModePrefix + to_string(mcs));
    uint64_t dataRate = mode.GetPhyRate(2160);

    // Create nodes
    NodeContainer nodes;
    nodes.Create(2);
    Ptr<Node> node1 = nodes.Get(0);
    Ptr<Node> node2 = nodes.Get(1);

    // WiGig setup
    WigigHelper wigig;
    
    WigigChannelHelper wigigChannel;
    wigigChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    wigigChannel.AddPropagationLoss("ns3::FriisPropagationLossModel",
                                     "Frequency", DoubleValue(60.48e9));

    WigigPhyHelper wigigPhy(wigigErrorModel);
    wigigPhy.SetChannel(wigigChannel.Create());
    wigigPhy.Set("TxPowerStart", DoubleValue(10.0));
    wigigPhy.Set("TxPowerEnd", DoubleValue(10.0));
    wigigPhy.Set("TxPowerLevels", UintegerValue(1));
    wigigPhy.Set("ChannelNumber", UintegerValue(channel));
    wigigPhy.Set("SupportOfdmPhy", BooleanValue(true));

    wigig.SetRemoteStationManager("ns3::ConstantRateWigigManager",
                                   "DataMode", StringValue(wigigModePrefix + to_string(mcs)));

    WigigMacHelper wigigMac;
    wigig.SetCodebook("ns3::CodebookAnalytical",
                      "CodebookType", EnumValue(SIMPLE_CODEBOOK),
                      "Antennas", UintegerValue(1),
                      "Sectors", UintegerValue(8));
    
    wigigMac.SetType("ns3::AdhocWigigMac",
                     "BE_MaxAmpduSize", StringValue(mpduAggSize),
                     "BE_MaxAmsduSize", StringValue(msduAggSize));

    // Install devices separately
    NetDeviceContainer device1 = wigig.Install(wigigPhy, wigigMac, node1);
    NetDeviceContainer device2 = wigig.Install(wigigPhy, wigigMac, node2);

    // Store PHY pointers
    phyA = DynamicCast<WigigNetDevice>(device1.Get(0))->GetPhy();
    phyB = DynamicCast<WigigNetDevice>(device2.Get(0))->GetPhy();

    // Configure antennas
    Simulator::ScheduleNow(&SetAntennaConfigurations, device1, device2);

    // Mobility
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(distance, 0.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    // Internet stack
    InternetStackHelper internet;
    internet.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.0.0.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces1 = address.Assign(device1);
    Ipv4InterfaceContainer interfaces2 = address.Assign(device2);

    std::cout << "Node 1: " << interfaces1.GetAddress(0) << std::endl;
    std::cout << "Node 2: " << interfaces2.GetAddress(0) << "\n" << std::endl;

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    PopulateArpCache();

    // Applications
    uint16_t port = 9999;
    
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                         InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApp = sink.Install(node1);
    sinkApp.Start(Seconds(0.0));

    OnOffHelper onoff("ns3::UdpSocketFactory",
                     InetSocketAddress(interfaces1.GetAddress(0), port));
    onoff.SetAttribute("PacketSize", UintegerValue(payloadSize));
    onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1e6]"));
    onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    onoff.SetAttribute("DataRate", DataRateValue(DataRate(dataRate)));
    
    ApplicationContainer sourceApp = onoff.Install(node2);
    sourceApp.Start(Seconds(0.0));
    sourceApp.Stop(Seconds(simulationTime));

    // Schedule events
    Simulator::Schedule(Seconds(obstructionStart), &BlockLink);
    Simulator::Schedule(Seconds(obstructionEnd), &UnblockLink);

    // Flow monitor
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    std::cout << "Starting simulation...\n" << std::endl;
    Simulator::Stop(Seconds(simulationTime));
    Simulator::Run();

    // Results
    std::cout << "\n======================================" << std::endl;
    std::cout << "           RESULTS" << std::endl;
    std::cout << "======================================\n" << std::endl;

    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();

    for (auto const &flow : stats)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(flow.first);
        double throughput = flow.second.rxBytes * 8.0 / simulationTime / 1e6;
        double loss = flow.second.txPackets > 0 ? 
                     ((flow.second.txPackets - flow.second.rxPackets) * 100.0) / flow.second.txPackets : 0;
        
        std::cout << "Flow: " << t.sourceAddress << " -> " << t.destinationAddress << std::endl;
        std::cout << "  Throughput: " << std::fixed << std::setprecision(2) 
                  << throughput << " Mbps" << std::endl;
        std::cout << "  TX: " << flow.second.txPackets << std::endl;
        std::cout << "  RX: " << flow.second.rxPackets << std::endl;
        std::cout << "  Loss: " << std::setprecision(2) << loss << " %\n" << std::endl;
    }

    std::cout << "======================================\n" << std::endl;

    Simulator::Destroy();
    return 0;
}
