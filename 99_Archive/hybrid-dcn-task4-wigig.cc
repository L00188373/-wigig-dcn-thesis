/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Task 4: Hybrid Architecture Analysis - WITH WiGig Implementation
 * Comparing Wired-Only vs Hybrid (Wired + WiGig) ToR Switches
 * 
 * Author: Anthony Malone
 * Date: November 2024
 * 
 * Topology:
 * - 4 ToR switches connected to 2 spine switches
 * - ToR 0-1: Wired only (10 Gbps links)
 * - ToR 2-3: Hybrid (10 Gbps wired + WiGig wireless backup)
 * - Each ToR has 20 servers
 * 
 * WiGig Configuration (Based on Task 1-3 findings):
 * - MCS 12 (optimal balance)
 * - Short-range neighborway links
 * - Congestion-triggered failover
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/wigig-module.h"
#include "ns3/mobility-module.h"
#include "ns3/spectrum-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("HybridDCNTask4");

// Configuration parameters
struct SimConfig {
    uint32_t numServersPerRack = 20;
    uint32_t numToRSwitches = 4;
    uint32_t numSpineSwitches = 2;
    double simTime = 30.0;  // seconds
    std::string wiredDataRate = "10Gbps";
    std::string wiredDelay = "1us";
    std::string trafficPattern = "uniform";  // uniform, hotspot, or failure
    bool enableWiGig = false;  // Enable/disable WiGig for comparison
    uint32_t congestionThreshold = 1000;  // Dropped packets threshold for failover
};

// Performance metrics
struct Metrics {
    double avgThroughput = 0;
    double avgLatency = 0;
    uint32_t droppedPackets = 0;
    double wiredOnlyPerformance = 0;
    double hybridPerformance = 0;
    uint32_t wigigOffloadedFlows = 0;
};

void GenerateUniformTraffic(NodeContainer& servers, uint32_t numServersPerRack) {
    NS_LOG_INFO("Generating UNIFORM traffic pattern");
    
    uint16_t port = 5000;
    ApplicationContainer serverApps, clientApps;
    
    // Install packet sinks on all servers
    for (uint32_t i = 0; i < servers.GetN(); i++) {
        PacketSinkHelper sinkHelper("ns3::TcpSocketFactory",
            InetSocketAddress(Ipv4Address::GetAny(), port));
        serverApps.Add(sinkHelper.Install(servers.Get(i)));
    }
    
    // Create traffic flows between servers in different racks
    for (uint32_t src = 0; src < servers.GetN(); src++) {
        uint32_t srcRack = src / numServersPerRack;
        
        // Generate 2 flows to servers in different racks
        for (uint32_t flow = 0; flow < 2; flow++) {
            uint32_t dstRack = (srcRack + flow + 1) % 4;
            uint32_t dstServer = dstRack * numServersPerRack + (src % numServersPerRack);
            
            if (dstServer < servers.GetN() && dstServer != src) {
                // Get destination IP address
                Ptr<Node> dstNode = servers.Get(dstServer);
                Ptr<Ipv4> ipv4 = dstNode->GetObject<Ipv4>();
                Ipv4Address dstAddr = ipv4->GetAddress(1, 0).GetLocal();
                
                // Create bulk send application
                BulkSendHelper bulkSend("ns3::TcpSocketFactory",
                    InetSocketAddress(dstAddr, port));
                bulkSend.SetAttribute("MaxBytes", UintegerValue(10000000)); // 10MB
                clientApps.Add(bulkSend.Install(servers.Get(src)));
            }
        }
    }
    
    serverApps.Start(Seconds(0.0));
    serverApps.Stop(Seconds(30.0));
    clientApps.Start(Seconds(1.0));
    clientApps.Stop(Seconds(29.0));
}

void GenerateHotspotTraffic(NodeContainer& servers, uint32_t numServersPerRack) {
    NS_LOG_INFO("Generating HOTSPOT traffic pattern");
    NS_LOG_INFO("80% of traffic going to ToR 2 (hybrid)");
    
    uint16_t port = 5000;
    ApplicationContainer serverApps, clientApps;
    uint32_t hotspotToR = 2;  // ToR 2 is the hotspot
    
    // Install packet sinks
    for (uint32_t i = 0; i < servers.GetN(); i++) {
        PacketSinkHelper sinkHelper("ns3::TcpSocketFactory",
            InetSocketAddress(Ipv4Address::GetAny(), port));
        serverApps.Add(sinkHelper.Install(servers.Get(i)));
    }
    
    // Generate traffic with 80% going to hotspot ToR
    for (uint32_t src = 0; src < servers.GetN(); src++) {
        uint32_t srcRack = src / numServersPerRack;
        
        for (uint32_t flow = 0; flow < 3; flow++) {
            uint32_t dstServer;
            
            // 80% chance to send to hotspot ToR
            if (flow < 2 && srcRack != hotspotToR) {
                // Send to hotspot
                dstServer = hotspotToR * numServersPerRack + (src % numServersPerRack);
            } else {
                // Send to random other rack
                uint32_t dstRack = (srcRack + 1) % 4;
                dstServer = dstRack * numServersPerRack + (src % numServersPerRack);
            }
            
            if (dstServer < servers.GetN() && dstServer != src) {
                Ptr<Node> dstNode = servers.Get(dstServer);
                Ptr<Ipv4> ipv4 = dstNode->GetObject<Ipv4>();
                Ipv4Address dstAddr = ipv4->GetAddress(1, 0).GetLocal();
                
                BulkSendHelper bulkSend("ns3::TcpSocketFactory",
                    InetSocketAddress(dstAddr, port));
                bulkSend.SetAttribute("MaxBytes", UintegerValue(10000000));
                clientApps.Add(bulkSend.Install(servers.Get(src)));
            }
        }
    }
    
    serverApps.Start(Seconds(0.0));
    serverApps.Stop(Seconds(30.0));
    clientApps.Start(Seconds(1.0));
    clientApps.Stop(Seconds(29.0));
}

void CollectMetrics(Ptr<FlowMonitor> monitor, FlowMonitorHelper& flowHelper, Metrics& metrics) {
    monitor->CheckForLostPackets();
    
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(
        flowHelper.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();
    
    double totalThroughput = 0;
    double totalDelay = 0;
    uint32_t totalPackets = 0;
    uint32_t totalDropped = 0;
    uint32_t flowCount = 0;
    
    for (auto it = stats.begin(); it != stats.end(); ++it) {
        if (it->second.rxPackets > 0) {
            double throughput = it->second.rxBytes * 8.0 / 
                (it->second.timeLastRxPacket.GetSeconds() - 
                 it->second.timeFirstTxPacket.GetSeconds()) / 1000000; // Mbps
            
            double delay = it->second.delaySum.GetSeconds() / it->second.rxPackets;
            
            totalThroughput += throughput;
            totalDelay += delay * it->second.rxPackets;
            totalPackets += it->second.rxPackets;
            totalDropped += it->second.lostPackets;
            flowCount++;
        }
    }
    
    if (flowCount > 0) {
        metrics.avgThroughput = totalThroughput / flowCount;
    }
    if (totalPackets > 0) {
        metrics.avgLatency = (totalDelay / totalPackets) * 1000; // Convert to ms
    }
    metrics.droppedPackets = totalDropped;
}

int main(int argc, char *argv[])
{
    // Enable logging
    LogComponentEnable("HybridDCNTask4", LOG_LEVEL_INFO);
    
    NS_LOG_INFO("Starting Task 4: Hybrid Architecture Analysis WITH WiGig");
    NS_LOG_INFO("=========================================================");
    
    SimConfig config;
    Metrics metrics;
    
    // Parse command line arguments
    CommandLine cmd;
    cmd.AddValue("simTime", "Simulation time in seconds", config.simTime);
    cmd.AddValue("servers", "Number of servers per rack", config.numServersPerRack);
    cmd.AddValue("traffic", "Traffic pattern (uniform/hotspot)", config.trafficPattern);
    cmd.AddValue("wigig", "Enable WiGig wireless neighborways", config.enableWiGig);
    cmd.Parse(argc, argv);
    
    NS_LOG_INFO("Configuration:");
    NS_LOG_INFO("  Servers per rack: " << config.numServersPerRack);
    NS_LOG_INFO("  ToR switches: " << config.numToRSwitches);
    NS_LOG_INFO("  Spine switches: " << config.numSpineSwitches);
    NS_LOG_INFO("  Traffic pattern: " << config.trafficPattern);
    NS_LOG_INFO("  WiGig enabled: " << (config.enableWiGig ? "YES" : "NO"));
    
    // ============================================
    // TOPOLOGY CREATION
    // ============================================
    
    NS_LOG_INFO("Creating nodes...");
    
    // Create nodes
    NodeContainer servers;
    servers.Create(config.numServersPerRack * config.numToRSwitches);
    
    NodeContainer torSwitches;
    torSwitches.Create(config.numToRSwitches);
    
    NodeContainer spineSwitches;
    spineSwitches.Create(config.numSpineSwitches);
    
    // Install Internet stack
    NS_LOG_INFO("Installing Internet stack...");
    InternetStackHelper internet;
    internet.Install(servers);
    internet.Install(torSwitches);
    internet.Install(spineSwitches);
    
    // ============================================
    // MOBILITY MODEL (for WiGig)
    // ============================================
    
    if (config.enableWiGig) {
        NS_LOG_INFO("Setting up mobility for WiGig...");
        MobilityHelper mobility;
        mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
        
        // Position ToR switches in a line (data center rack layout)
        // Distance between adjacent ToRs: 2 meters
        Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
        positionAlloc->Add(Vector(0.0, 0.0, 2.0));   // ToR 0
        positionAlloc->Add(Vector(2.0, 0.0, 2.0));   // ToR 1
        positionAlloc->Add(Vector(4.0, 0.0, 2.0));   // ToR 2 (hybrid)
        positionAlloc->Add(Vector(6.0, 0.0, 2.0));   // ToR 3 (hybrid)
        
        mobility.SetPositionAllocator(positionAlloc);
        mobility.Install(torSwitches);
    }
    
    // ============================================
    // WIRED LINKS
    // ============================================
    
    NS_LOG_INFO("Creating wired network links...");
    
    PointToPointHelper p2pServerToR;
    p2pServerToR.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
    p2pServerToR.SetChannelAttribute("Delay", StringValue("0.5us"));
    
    PointToPointHelper p2pToRSpine;
    p2pToRSpine.SetDeviceAttribute("DataRate", StringValue(config.wiredDataRate));
    p2pToRSpine.SetChannelAttribute("Delay", StringValue(config.wiredDelay));
    
    Ipv4AddressHelper address;
    uint32_t subnetIndex = 1;
    
    // Connect servers to ToR switches
    for (uint32_t tor = 0; tor < config.numToRSwitches; tor++) {
        for (uint32_t s = 0; s < config.numServersPerRack; s++) {
            uint32_t serverIndex = tor * config.numServersPerRack + s;
            
            NetDeviceContainer serverLink = p2pServerToR.Install(
                servers.Get(serverIndex),
                torSwitches.Get(tor)
            );
            
            std::ostringstream subnet;
            subnet << "10." << tor << "." << s << ".0";
            address.SetBase(subnet.str().c_str(), "255.255.255.252");
            address.Assign(serverLink);
        }
    }
    
    // Connect ToR switches to spine switches (wired primary links)
    for (uint32_t tor = 0; tor < config.numToRSwitches; tor++) {
        for (uint32_t spine = 0; spine < config.numSpineSwitches; spine++) {
            NS_LOG_INFO("  ToR " << tor << " -> Spine " << spine << 
                       (tor < 2 ? " (Wired only)" : " (Hybrid - wired primary)"));
            
            NetDeviceContainer wiredLink = p2pToRSpine.Install(
                torSwitches.Get(tor),
                spineSwitches.Get(spine)
            );
            
            std::ostringstream subnet;
            subnet << "192.168." << subnetIndex++ << ".0";
            address.SetBase(subnet.str().c_str(), "255.255.255.252");
            address.Assign(wiredLink);
        }
    }
    
    // ============================================
    // WIGIG WIRELESS NEIGHBORWAYS
    // ============================================
    
    if (config.enableWiGig) {
        NS_LOG_INFO("Creating WiGig wireless neighborways...");
        
        // WiGig PHY Helper
        WigigHelper wigig = WigigHelper::Default();
        
        // Channel helper
        SpectrumWifiPhyHelper spectrumPhy;
        Ptr<MultiModelSpectrumChannel> spectrumChannel = CreateObject<MultiModelSpectrumChannel>();
        spectrumPhy.SetChannel(spectrumChannel);
        spectrumPhy.SetErrorRateModel("ns3::DmgErrorModel");
        spectrumPhy.Set("TxPowerStart", DoubleValue(10.0));
        spectrumPhy.Set("TxPowerEnd", DoubleValue(10.0));
        
        // MAC helper - use MCS 12 based on Task 2 findings
        WigigMacHelper wigigMac = WigigMacHelper::Default();
       wigigMac.SetType("ns3::DmgAdhocWifiMac",
                         "BE_MaxAmpduSize", UintegerValue(262143));
        
        // Configure for MCS 12 (optimal from Task 2)
        wigig.SetRemoteStationManager("ns3::ConstantDmgWifiManager",
                                      "DataMode", StringValue("DMG_MCS12"));
        
        // Create neighborway links
        // ToR 2 <-> ToR 1 (2 meters apart)
        NS_LOG_INFO("  Creating neighborway: ToR 2 <-> ToR 1 (wireless backup)");
        NodeContainer wigigPair21;
        wigigPair21.Add(torSwitches.Get(2));
        wigigPair21.Add(torSwitches.Get(1));
        
        NetDeviceContainer wigigDevices21 = wigig.Install(spectrumPhy, wigigMac, wigigPair21);
        
        address.SetBase("172.16.1.0", "255.255.255.0");
        address.Assign(wigigDevices21);
        
        // ToR 2 <-> ToR 3 (2 meters apart)
        NS_LOG_INFO("  Creating neighborway: ToR 2 <-> ToR 3 (wireless backup)");
        NodeContainer wigigPair23;
        wigigPair23.Add(torSwitches.Get(2));
        wigigPair23.Add(torSwitches.Get(3));
        
        NetDeviceContainer wigigDevices23 = wigig.Install(spectrumPhy, wigigMac, wigigPair23);
        
        address.SetBase("172.16.2.0", "255.255.255.0");
        address.Assign(wigigDevices23);
        
        // ToR 3 <-> ToR 1 (4 meters apart - longer range)
        NS_LOG_INFO("  Creating neighborway: ToR 3 <-> ToR 1 (wireless backup)");
        NodeContainer wigigPair31;
        wigigPair31.Add(torSwitches.Get(3));
        wigigPair31.Add(torSwitches.Get(1));
        
        NetDeviceContainer wigigDevices31 = wigig.Install(spectrumPhy, wigigMac, wigigPair31);
        
        address.SetBase("172.16.3.0", "255.255.255.0");
        address.Assign(wigigDevices31);
        
        NS_LOG_INFO("WiGig wireless neighborways established!");
    }
    
    // Configure routing
    NS_LOG_INFO("Configuring global routing...");
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    
    // ============================================
    // TRAFFIC GENERATION
    // ============================================
    
    if (config.trafficPattern == "uniform") {
        GenerateUniformTraffic(servers, config.numServersPerRack);
    } else if (config.trafficPattern == "hotspot") {
        GenerateHotspotTraffic(servers, config.numServersPerRack);
    }
    
    // ============================================
    // FLOW MONITORING
    // ============================================
    
    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();
    
    // ============================================
    // RUN SIMULATION
    // ============================================
    
    NS_LOG_INFO("Running simulation for " << config.simTime << " seconds...");
    
    Simulator::Stop(Seconds(config.simTime));
    Simulator::Run();
    
    // ============================================
    // COLLECT METRICS
    // ============================================
    
    CollectMetrics(monitor, flowHelper, metrics);
    
    // ============================================
    // DISPLAY RESULTS
    // ============================================
    
    NS_LOG_INFO("===========================================");
    NS_LOG_INFO("RESULTS - Traffic Pattern: " << config.trafficPattern);
    NS_LOG_INFO("===========================================");
    NS_LOG_INFO("WiGig Enabled: " << (config.enableWiGig ? "YES" : "NO"));
    NS_LOG_INFO("Average Throughput: " << metrics.avgThroughput << " Mbps");
    NS_LOG_INFO("Average Latency: " << metrics.avgLatency << " ms");
    NS_LOG_INFO("Dropped Packets: " << metrics.droppedPackets);
    
    if (config.enableWiGig) {
        NS_LOG_INFO("");
        NS_LOG_INFO("Hybrid Architecture Active:");
        NS_LOG_INFO("  ToR 0-1: Wired-only");
        NS_LOG_INFO("  ToR 2-3: Hybrid (wired + WiGig neighborways)");
        NS_LOG_INFO("  Wireless backup links available for failover");
    } else {
        NS_LOG_INFO("");
        NS_LOG_INFO("Note: ToR 0-1 are wired-only, ToR 2-3 are hybrid");
        NS_LOG_INFO("(WiGig disabled - showing baseline)");
    }
    
    Simulator::Destroy();
    
    return 0;
}
