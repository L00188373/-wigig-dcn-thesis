/*
 * 60 GHz WiGig + Cat6 Ethernet Hybrid Data Center Network Simulation
 * 
 * Architecture:
 * - Dual-path topology: Wired (Cat6 1Gbps) + Wireless (60 GHz WiGig)
 * - Traffic classification: Mice flows (<1KB) via wired, Elephant flows (≥1KB) via wireless
 * - Measures throughput, latency, packet loss for both paths
 * 
 * Author: L00188373
 * Date: November 2025
 */

#include "wigig-examples-common-functions.h"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wigig-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/point-to-point-module.h"

#include <iomanip>
#include <string>

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE("DcnHybridRouting");

// Application variables
Ptr<PacketSink> wiredSink;      // For mice flows (wired path)
Ptr<PacketSink> wirelessSink;   // For elephant flows (wireless path)

// Network nodes
Ptr<Node> serverA, serverB;

// Function to configure WiGig antenna settings
void
SetAntennaConfigurations(NetDeviceContainer& deviceA, NetDeviceContainer& deviceB)
{
    Ptr<WigigNetDevice> wigigDeviceA = DynamicCast<WigigNetDevice>(deviceA.Get(0));
    Ptr<WigigNetDevice> wigigDeviceB = DynamicCast<WigigNetDevice>(deviceB.Get(0));
    Ptr<AdhocWigigMac> wigigMacA = DynamicCast<AdhocWigigMac>(wigigDeviceA->GetMac());
    Ptr<AdhocWigigMac> wigigMacB = DynamicCast<AdhocWigigMac>(wigigDeviceB->GetMac());
    
    // Configure antenna patterns
    wigigMacA->AddAntennaConfig(1, 1, wigigMacB->GetAddress());
    wigigMacB->AddAntennaConfig(5, 1, wigigMacA->GetAddress());
    
    // Steer antennas toward each other
    wigigMacA->SteerAntennaToward(wigigMacB->GetAddress());
    wigigMacB->SteerAntennaToward(wigigMacA->GetAddress());
}

int main(int argc, char *argv[])
{
    // ========================================================================
    // Simulation Parameters
    // ========================================================================
    
    // WiGig parameters
    uint32_t payloadSize = 1472;                // Application payload size in bytes
    string msduAggSize = "max";                 // Maximum A-MSDU aggregation
    string mpduAggSize = "max";                 // Maximum A-MPDU aggregation
    string queueSize = "4000p";                 // WiGig MAC queue size
    uint32_t channel = 2;                       // WiGig channel number
    bool enableRts = false;                     // RTS/CTS handshake
    uint32_t rtsThreshold = 0;                  // RTS/CTS threshold
    uint32_t mcs = 12;                          // MCS 12 (optimal from previous results)
    string wigigErrorModel = "src/wigig/model/reference/ErrorModel/LookupTable_1458.txt";
    
    // Topology parameters
    double distance = 1.0;                      // Distance between servers (meters)
    double simulationTime = 10.0;               // Simulation duration (seconds)
    
    // Traffic parameters
    uint32_t micePacketSize = 512;              // Mice flow packet size (bytes)
    uint32_t elephantPacketSize = 65000;        // Elephant flow packet size (bytes)
    double miceDataRate = 100;                  // Mice flow data rate (Mbps)
    double elephantDataRate = 2000;             // Elephant flow data rate (Mbps)
    
    // Wired path parameters
    string wiredDataRate = "1Gbps";             // Cat6 Ethernet rate
    string wiredDelay = "0.5ms";                // Cat6 latency
    
    // Output parameters
    bool verbose = false;
    string outputPrefix = "hybrid-results";

    // Command line arguments
    CommandLine cmd;
    cmd.AddValue("mcs", "WiGig MCS index (1-24)", mcs);
    cmd.AddValue("payloadSize", "Application payload size in bytes", payloadSize);
    cmd.AddValue("msduAggSize", "Maximum A-MSDU aggregation size", msduAggSize);
    cmd.AddValue("mpduAggSize", "Maximum A-MPDU aggregation size", mpduAggSize);
    cmd.AddValue("queueSize", "WiGig MAC queue size", queueSize);
    cmd.AddValue("channel", "WiGig channel number", channel);
    cmd.AddValue("simulationTime", "Simulation time in seconds", simulationTime);
    cmd.AddValue("distance", "Distance between servers in meters", distance);
    cmd.AddValue("micePacketSize", "Mice flow packet size in bytes", micePacketSize);
    cmd.AddValue("elephantPacketSize", "Elephant flow packet size in bytes", elephantPacketSize);
    cmd.AddValue("miceDataRate", "Mice flow data rate in Mbps", miceDataRate);
    cmd.AddValue("elephantDataRate", "Elephant flow data rate in Mbps", elephantDataRate);
    cmd.AddValue("wiredDataRate", "Wired link data rate (e.g., 1Gbps)", wiredDataRate);
    cmd.AddValue("wiredDelay", "Wired link delay (e.g., 0.5ms)", wiredDelay);
    cmd.AddValue("enableRts", "Enable RTS/CTS handshake", enableRts);
    cmd.AddValue("errorModel", "Path to WiGig error model", wigigErrorModel);
    cmd.AddValue("verbose", "Enable verbose logging", verbose);
    cmd.AddValue("outputPrefix", "Output file prefix", outputPrefix);
    cmd.Parse(argc, argv);

    if (verbose)
    {
        LogComponentEnable("DcnHybridRouting", LOG_LEVEL_INFO);
    }
    
    // Validate and configure frame aggregation
    ValidateFrameAggregationAttributes(msduAggSize, mpduAggSize);
    ConfigureRtsCtsAndFragmenatation(enableRts, rtsThreshold);
    ChangeQueueSize(queueSize);

    std::cout << "\n╔════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║   Hybrid DCN: Cat6 Ethernet + 60 GHz WiGig            ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════╝\n" << std::endl;
    std::cout << "Architecture: Dual-path with traffic classification" << std::endl;
    std::cout << "  Wired path:   Cat6 Ethernet @ " << wiredDataRate << std::endl;
    std::cout << "  Wireless path: 60 GHz WiGig @ MCS " << mcs << std::endl;
    std::cout << "  Distance:     " << distance << " meters\n" << std::endl;
    std::cout << "Traffic Classification:" << std::endl;
    std::cout << "  Mice flows (≤1KB):    Wired path (low latency)" << std::endl;
    std::cout << "  Elephant flows (>1KB): Wireless path (high throughput)\n" << std::endl;

    std::cout << "Traffic Classification:" << std::endl;
    std::cout << "  Mice flows (≤1KB):    Wired path (low latency)" << std::endl;
    std::cout << "  Elephant flows (>1KB): Wireless path (high throughput)\n" << std::endl;

    // ========================================================================
    // PHASE 1: Create Network Nodes
    // ========================================================================
    
    NS_LOG_INFO("Creating 2-node topology...");
    
    NodeContainer nodes;
    nodes.Create(2);
    serverA = nodes.Get(0);
    serverB = nodes.Get(1);

    // ========================================================================
    // PHASE 2: Configure Wired Path (Cat6 Ethernet)
    // ========================================================================
    
    NS_LOG_INFO("Configuring wired path (Cat6 Ethernet)...");
    
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue(wiredDataRate));
    p2p.SetChannelAttribute("Delay", StringValue(wiredDelay));
    
    NetDeviceContainer wiredDevices = p2p.Install(nodes);

    // ========================================================================
    // PHASE 3: Configure Wireless Path (60 GHz WiGig)
    // ========================================================================
    
    NS_LOG_INFO("Configuring wireless path (60 GHz WiGig)...");
    
    // WiGig mode string
    string wigigModePrefix = "DmgMcs";
    WifiMode wigigMode = WifiMode(wigigModePrefix + std::to_string(mcs));
    // uint64_t wigigDataRate = wigigMode.GetPhyRate(2160);  // Get nominal PHY rate
    
    // WigigHelper setup
    WigigHelper wigig;
    
    // Channel configuration
    WigigChannelHelper wigigChannel;
    wigigChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    wigigChannel.AddPropagationLoss("ns3::FriisPropagationLossModel",
                                     "Frequency",
                                     DoubleValue(60.48e9));  // 60 GHz
    
    // PHY configuration
    WigigPhyHelper wigigPhy(wigigErrorModel);
    wigigPhy.SetChannel(wigigChannel.Create());
    wigigPhy.Set("TxPowerStart", DoubleValue(0.0));
    wigigPhy.Set("TxPowerEnd", DoubleValue(0.0));
    wigigPhy.Set("TxPowerLevels", UintegerValue(1));
    wigigPhy.Set("ChannelNumber", UintegerValue(channel));
    wigigPhy.Set("SupportOfdmPhy", BooleanValue(true));
    
    // Set constant rate manager
    wigig.SetRemoteStationManager("ns3::ConstantRateWigigManager",
                                   "DataMode",
                                   StringValue(wigigModePrefix + std::to_string(mcs)));
    
    // MAC configuration
    WigigMacHelper wigigMac;
    
    // Set analytical codebook
    wigig.SetCodebook("ns3::CodebookAnalytical",
                      "CodebookType",
                      EnumValue(SIMPLE_CODEBOOK),
                      "Antennas",
                      UintegerValue(1),
                      "Sectors",
                      UintegerValue(8));
    
    // Create WiGig devices with Ad-hoc MAC
    wigigMac.SetType("ns3::AdhocWigigMac",
                     "BE_MaxAmpduSize",
                     StringValue(mpduAggSize),
                     "BE_MaxAmsduSize",
                     StringValue(msduAggSize));
    
    NetDeviceContainer wirelessDeviceA = wigig.Install(wigigPhy, wigigMac, serverA);
    NetDeviceContainer wirelessDeviceB = wigig.Install(wigigPhy, wigigMac, serverB);
    
    // Set antenna configurations
    Simulator::ScheduleNow(&SetAntennaConfigurations, wirelessDeviceA, wirelessDeviceB);
    
    // Combine wireless devices for easier management
    NetDeviceContainer wirelessDevices;
    wirelessDevices.Add(wirelessDeviceA);
    wirelessDevices.Add(wirelessDeviceB);
    
    // Mobility model - fixed positions
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));       // Server A
    positionAlloc->Add(Vector(distance, 0.0, 0.0));  // Server B
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    mobility.Install(nodes);
    // PHASE 4: Install Internet Stack and Assign IP Addresses
    // ========================================================================
    
    NS_LOG_INFO("Installing Internet stack and assigning IP addresses...");
    
    InternetStackHelper internet;
    internet.Install(nodes);

    // Wired network: 10.1.1.0/24
    Ipv4AddressHelper addressWired;
    addressWired.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer wiredInterfaces = addressWired.Assign(wiredDevices);

    // Wireless network: 10.1.2.0/24
    Ipv4AddressHelper addressWireless;
    addressWireless.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer wirelessInterfaces = addressWireless.Assign(wirelessDevices);

    std::cout << "\nIP Address Assignment:" << std::endl;
    std::cout << "  Server A (Wired):    " << wiredInterfaces.GetAddress(0) << std::endl;
    std::cout << "  Server B (Wired):    " << wiredInterfaces.GetAddress(1) << std::endl;
    std::cout << "  Server A (Wireless): " << wirelessInterfaces.GetAddress(0) << std::endl;
    std::cout << "  Server B (Wireless): " << wirelessInterfaces.GetAddress(1) << "\n" << std::endl;
    // Populate routing tables
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    
    // Populate ARP cache to avoid ARP packets
    PopulateArpCache();

    // Populate ARP cache to avoid ARP packets
    PopulateArpCache();

    // ========================================================================
    // PHASE 5: Setup Applications - Traffic Classification
    // ========================================================================
    
    NS_LOG_INFO("Setting up traffic applications...");
    
    std::cout << "Setting up traffic flows..." << std::endl;

    // --------------------------------------------------
    // MICE FLOWS: Small packets via WIRED path
    // --------------------------------------------------
    
    uint16_t micePort = 9000;
    // Packet sink on Server B (wired interface)
    PacketSinkHelper miceSinkHelper("ns3::UdpSocketFactory",
                                     InetSocketAddress(Ipv4Address::GetAny(), micePort));
    ApplicationContainer miceSinkApp = miceSinkHelper.Install(serverB);
    miceSinkApp.Start(Seconds(0.0));
    miceSinkApp.Stop(Seconds(simulationTime));
    wiredSink = StaticCast<PacketSink>(miceSinkApp.Get(0));

    // OnOff application on Server A (sending to wired interface of Server B)
    OnOffHelper miceOnOff("ns3::UdpSocketFactory",
                          InetSocketAddress(wiredInterfaces.GetAddress(1), micePort));
    miceOnOff.SetAttribute("PacketSize", UintegerValue(micePacketSize));
    miceOnOff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1e6]"));
    miceOnOff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    miceOnOff.SetAttribute("DataRate", DataRateValue(DataRate(miceDataRate * 1e6)));
    
    ApplicationContainer miceSourceApp = miceOnOff.Install(serverA);
    miceSourceApp.Start(Seconds(0.1));
    miceSourceApp.Stop(Seconds(simulationTime - 0.1));
    
    std::cout << "  Mice flow:     " << wiredInterfaces.GetAddress(0) << " -> " 
              << wiredInterfaces.GetAddress(1) << " (wired, " << micePacketSize << "B @ " 
              << miceDataRate << " Mbps)" << std::endl;

    // --------------------------------------------------
    // ELEPHANT FLOWS: Large packets via WIRELESS path
    // --------------------------------------------------
    
    uint16_t elephantPort = 9001;
    
    // Packet sink on Server B (wireless interface)
    PacketSinkHelper elephantSinkHelper("ns3::UdpSocketFactory",
                                         InetSocketAddress(Ipv4Address::GetAny(), elephantPort));
    ApplicationContainer elephantSinkApp = elephantSinkHelper.Install(serverB);
    elephantSinkApp.Start(Seconds(0.0));
    elephantSinkApp.Stop(Seconds(simulationTime));
    wirelessSink = StaticCast<PacketSink>(elephantSinkApp.Get(0));

    // OnOff application on Server A (sending to wireless interface of Server B)
    OnOffHelper elephantOnOff("ns3::UdpSocketFactory",
                              InetSocketAddress(wirelessInterfaces.GetAddress(1), elephantPort));
    elephantOnOff.SetAttribute("PacketSize", UintegerValue(elephantPacketSize));
    elephantOnOff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1e6]"));
    elephantOnOff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    elephantOnOff.SetAttribute("DataRate", DataRateValue(DataRate(elephantDataRate * 1e6)));
    
    ApplicationContainer elephantSourceApp = elephantOnOff.Install(serverA);
    elephantSourceApp.Start(Seconds(0.1));
    elephantSourceApp.Stop(Seconds(simulationTime - 0.1));
    
    std::cout << "  Elephant flow: " << wirelessInterfaces.GetAddress(0) << " -> " 
              << wirelessInterfaces.GetAddress(1) << " (wireless, " << elephantPacketSize 
              << "B @ " << elephantDataRate << " Mbps)\n" << std::endl;

    std::cout << "  Elephant flow: " << wirelessInterfaces.GetAddress(0) << " -> " 
              << wirelessInterfaces.GetAddress(1) << " (wireless, " << elephantPacketSize 
              << "B @ " << elephantDataRate << " Mbps)\n" << std::endl;

    // ========================================================================
    // PHASE 6: Setup Flow Monitor
    // ========================================================================
    
    NS_LOG_INFO("Installing Flow Monitor...");
    
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    // ========================================================================
    // PHASE 7: Run Simulation
    // ========================================================================
    
    NS_LOG_INFO("Starting simulation for " << simulationTime << " seconds...");
    std::cout << "Running simulation..." << std::flush;
    
    Simulator::Stop(Seconds(simulationTime));
    Simulator::Run();
    
    std::cout << " DONE\n" << std::endl;

    // ========================================================================
    // PHASE 8: Calculate and Display Results
    // ========================================================================
    
    NS_LOG_INFO("Simulation completed. Analyzing results...");

    // Get bytes received from packet sinks
    uint64_t wiredRxBytes = wiredSink->GetTotalRx();
    uint64_t wirelessRxBytes = wirelessSink->GetTotalRx();
    
    // Calculate throughput (exclude startup/shutdown time)
    double actualSimTime = simulationTime - 0.2;  // Exclude 0.1s startup and 0.1s shutdown
    double wiredThroughput = (wiredRxBytes * 8.0) / (actualSimTime * 1e6);  // Mbps
    double wirelessThroughput = (wirelessRxBytes * 8.0) / (actualSimTime * 1e6);  // Mbps
    double totalThroughput = wiredThroughput + wirelessThroughput;

    // Flow Monitor statistics
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();

    double wiredLatency = 0.0;
    double wirelessLatency = 0.0;
    double wiredPacketLoss = 0.0;
    double wirelessPacketLoss = 0.0;
    uint32_t wiredTxPackets = 0;
    uint32_t wiredRxPackets = 0;
    uint32_t wirelessTxPackets = 0;
    uint32_t wirelessRxPackets = 0;

    for (auto const &flow : stats)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(flow.first);
        
        // Identify flow by destination address and port
        if (t.destinationAddress == wiredInterfaces.GetAddress(1) && t.destinationPort == micePort)
        {
            // Wired flow (mice)
            wiredTxPackets = flow.second.txPackets;
            wiredRxPackets = flow.second.rxPackets;
            if (flow.second.rxPackets > 0)
            {
                wiredLatency = flow.second.delaySum.GetMilliSeconds() / flow.second.rxPackets;
            }
            if (flow.second.txPackets > 0)
            {
                wiredPacketLoss = ((flow.second.txPackets - flow.second.rxPackets) * 100.0) / 
                                   flow.second.txPackets;
            }
        }
        else if (t.destinationAddress == wirelessInterfaces.GetAddress(1) && 
                 t.destinationPort == elephantPort)
        {
            // Wireless flow (elephant)
            wirelessTxPackets = flow.second.txPackets;
            wirelessRxPackets = flow.second.rxPackets;
            if (flow.second.rxPackets > 0)
            {
                wirelessLatency = flow.second.delaySum.GetMilliSeconds() / flow.second.rxPackets;
            }
            if (flow.second.txPackets > 0)
            {
                wirelessPacketLoss = ((flow.second.txPackets - flow.second.rxPackets) * 100.0) / 
                                      flow.second.txPackets;
            }
        }
    }

    // ========================================================================
    // PHASE 9: Output Results
    // ========================================================================
    
    std::cout << "============================================================\n";
    std::cout << "           HYBRID ROUTING SIMULATION RESULTS\n";
    std::cout << "============================================================\n\n";
    
    std::cout << "Simulation Parameters:\n";
    std::cout << "  Duration:      " << simulationTime << " seconds\n";
    std::cout << "  Distance:      " << distance << " meters\n";
    std::cout << "  Wireless MCS:  " << mcs << "\n\n";

    std::cout << "WIRED PATH (Cat6 Ethernet) - Mice Flows (" << micePacketSize << "B packets):\n";
    std::cout << "  Target Rate:   " << miceDataRate << " Mbps\n";
    std::cout << "  Throughput:    " << std::fixed << std::setprecision(2) << wiredThroughput << " Mbps\n";
    std::cout << "  Packets TX:    " << wiredTxPackets << "\n";
    std::cout << "  Packets RX:    " << wiredRxPackets << "\n";
    std::cout << "  Packet Loss:   " << std::setprecision(2) << wiredPacketLoss << " %\n";
    std::cout << "  Avg Latency:   " << std::setprecision(3) << wiredLatency << " ms\n\n";

    std::cout << "WIRELESS PATH (60 GHz WiGig) - Elephant Flows (" << elephantPacketSize << "B packets):\n";
    std::cout << "  Target Rate:   " << elephantDataRate << " Mbps\n";
    std::cout << "  Throughput:    " << std::setprecision(2) << wirelessThroughput << " Mbps\n";
    std::cout << "  Packets TX:    " << wirelessTxPackets << "\n";
    std::cout << "  Packets RX:    " << wirelessRxPackets << "\n";
    std::cout << "  Packet Loss:   " << std::setprecision(2) << wirelessPacketLoss << " %\n";
    std::cout << "  Avg Latency:   " << std::setprecision(3) << wirelessLatency << " ms\n\n";

    std::cout << "HYBRID PERFORMANCE:\n";
    std::cout << "  Total Throughput:      " << std::setprecision(2) << totalThroughput << " Mbps\n";
    std::cout << "  Wired Contribution:    " << std::setprecision(1) 
              << (wiredThroughput / totalThroughput * 100) << " %\n";
    std::cout << "  Wireless Contribution: " << std::setprecision(1) 
              << (wirelessThroughput / totalThroughput * 100) << " %\n";
    std::cout << "  Speedup vs Cat6 only:  " << std::setprecision(2) 
              << (totalThroughput / 1000.0) << "×\n\n";
    // Save results to CSV file
    std::ofstream csvFile(outputPrefix + ".csv");
    csvFile << "Path,PacketSize,TargetRate,Throughput,PacketLoss,Latency,TxPackets,RxPackets\n";
    csvFile << "Wired," << micePacketSize << "," << miceDataRate << "," 
            << wiredThroughput << "," << wiredPacketLoss << "," << wiredLatency << ","
            << wiredTxPackets << "," << wiredRxPackets << "\n";
    csvFile << "Wireless," << elephantPacketSize << "," << elephantDataRate << "," 
            << wirelessThroughput << "," << wirelessPacketLoss << "," << wirelessLatency << ","
            << wirelessTxPackets << "," << wirelessRxPackets << "\n";
    csvFile << "Total,N/A,N/A," << totalThroughput << ",N/A,N/A,N/A,N/A\n";
    csvFile.close();

    std::cout << "Results saved to: " << outputPrefix << ".csv\n" << std::endl;

    Simulator::Destroy();

    return 0;
}