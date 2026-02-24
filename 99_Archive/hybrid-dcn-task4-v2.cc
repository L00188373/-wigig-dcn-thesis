/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Task 4: Hybrid Architecture Analysis - Simplified WiGig
 * Using high-speed P2P links to simulate wireless neighborways
 * 
 * Author: Anthony Malone
 * Date: November 2024
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("HybridDCNTask4");

// Configuration parameters
struct SimConfig {
    uint32_t numServersPerRack = 20;
    uint32_t numToRSwitches = 4;
    uint32_t numSpineSwitches = 2;
    double simTime = 30.0;
    std::string wiredDataRate = "10Gbps";
    std::string wiredDelay = "1us";
    std::string wirelessDataRate = "4Gbps";  // WiGig backup capacity
    std::string wirelessDelay = "2us";
    std::string trafficPattern = "uniform";
    bool enableWireless = false;
};

// Performance metrics
struct Metrics {
    double avgThroughput = 0;
    double avgLatency = 0;
    uint32_t droppedPackets = 0;
};

void GenerateUniformTraffic(NodeContainer& servers, uint32_t numServersPerRack) {
    NS_LOG_INFO("Generating UNIFORM traffic pattern");
    
    uint16_t port = 5000;
    ApplicationContainer serverApps, clientApps;
    
    for (uint32_t i = 0; i < servers.GetN(); i++) {
        PacketSinkHelper sinkHelper("ns3::TcpSocketFactory",
            InetSocketAddress(Ipv4Address::GetAny(), port));
        serverApps.Add(sinkHelper.Install(servers.Get(i)));
    }
    
    for (uint32_t src = 0; src < servers.GetN(); src++) {
        uint32_t srcRack = src / numServersPerRack;
        
        for (uint32_t flow = 0; flow < 2; flow++) {
            uint32_t dstRack = (srcRack + flow + 1) % 4;
            uint32_t dstServer = dstRack * numServersPerRack + (src % numServersPerRack);
            
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

void GenerateHotspotTraffic(NodeContainer& servers, uint32_t numServersPerRack) {
    NS_LOG_INFO("Generating HOTSPOT traffic pattern");
    NS_LOG_INFO("80% of traffic going to ToR 2 (hybrid)");
    
    uint16_t port = 5000;
    ApplicationContainer serverApps, clientApps;
    uint32_t hotspotToR = 2;
    
    for (uint32_t i = 0; i < servers.GetN(); i++) {
        PacketSinkHelper sinkHelper("ns3::TcpSocketFactory",
            InetSocketAddress(Ipv4Address::GetAny(), port));
        serverApps.Add(sinkHelper.Install(servers.Get(i)));
    }
    
    for (uint32_t src = 0; src < servers.GetN(); src++) {
        uint32_t srcRack = src / numServersPerRack;
        
        for (uint32_t flow = 0; flow < 3; flow++) {
            uint32_t dstServer;
            
            if (flow < 2 && srcRack != hotspotToR) {
                dstServer = hotspotToR * numServersPerRack + (src % numServersPerRack);
            } else {
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
                 it->second.timeFirstTxPacket.GetSeconds()) / 1000000;
            
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
        metrics.avgLatency = (totalDelay / totalPackets) * 1000;
    }
    metrics.droppedPackets = totalDropped;
}

int main(int argc, char *argv[])
{
    LogComponentEnable("HybridDCNTask4", LOG_LEVEL_INFO);
    
    NS_LOG_INFO("Starting Task 4: Hybrid Architecture Analysis");
    NS_LOG_INFO("===========================================");
    
    SimConfig config;
    Metrics metrics;
    
    CommandLine cmd;
    cmd.AddValue("simTime", "Simulation time", config.simTime);
    cmd.AddValue("servers", "Servers per rack", config.numServersPerRack);
    cmd.AddValue("traffic", "Traffic pattern (uniform/hotspot)", config.trafficPattern);
    cmd.AddValue("wireless", "Enable wireless backup links", config.enableWireless);
    cmd.Parse(argc, argv);
    
    NS_LOG_INFO("Configuration:");
    NS_LOG_INFO("  Servers per rack: " << config.numServersPerRack);
    NS_LOG_INFO("  ToR switches: " << config.numToRSwitches);
    NS_LOG_INFO("  Spine switches: " << config.numSpineSwitches);
    NS_LOG_INFO("  Traffic pattern: " << config.trafficPattern);
    NS_LOG_INFO("  Wireless backup: " << (config.enableWireless ? "ENABLED" : "DISABLED"));
    
    // Create nodes
    NS_LOG_INFO("Creating nodes...");
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
    
    // Create wired links
    NS_LOG_INFO("Creating network links...");
    
    PointToPointHelper p2pServerToR;
    p2pServerToR.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
    p2pServerToR.SetChannelAttribute("Delay", StringValue("0.5us"));
    
    PointToPointHelper p2pToRSpine;
    p2pToRSpine.SetDeviceAttribute("DataRate", StringValue(config.wiredDataRate));
    p2pToRSpine.SetChannelAttribute("Delay", StringValue(config.wiredDelay));
    
    Ipv4AddressHelper address;
    uint32_t subnetIndex = 1;
    
    // Connect servers to ToR
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
    
    // Connect ToR to Spine (primary wired links)
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
    
    // Add wireless backup links (neighborways) if enabled
    if (config.enableWireless) {
        NS_LOG_INFO("Creating wireless backup neighborways...");
        
        PointToPointHelper p2pWireless;
        p2pWireless.SetDeviceAttribute("DataRate", StringValue(config.wirelessDataRate));
        p2pWireless.SetChannelAttribute("Delay", StringValue(config.wirelessDelay));
        
        // ToR 2 <-> ToR 1
        NS_LOG_INFO("  Neighborway: ToR 2 <-> ToR 1 (4 Gbps wireless)");
        NetDeviceContainer wireless21 = p2pWireless.Install(
            torSwitches.Get(2), torSwitches.Get(1));
        address.SetBase("172.16.1.0", "255.255.255.0");
        address.Assign(wireless21);
        
        // ToR 2 <-> ToR 3
        NS_LOG_INFO("  Neighborway: ToR 2 <-> ToR 3 (4 Gbps wireless)");
        NetDeviceContainer wireless23 = p2pWireless.Install(
            torSwitches.Get(2), torSwitches.Get(3));
        address.SetBase("172.16.2.0", "255.255.255.0");
        address.Assign(wireless23);
        
        // ToR 3 <-> ToR 1
        NS_LOG_INFO("  Neighborway: ToR 3 <-> ToR 1 (4 Gbps wireless)");
        NetDeviceContainer wireless31 = p2pWireless.Install(
            torSwitches.Get(3), torSwitches.Get(1));
        address.SetBase("172.16.3.0", "255.255.255.0");
        address.Assign(wireless31);
        
        NS_LOG_INFO("Wireless neighborways established!");
    }
    
    // Configure routing
    NS_LOG_INFO("Configuring global routing...");
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    
    // Generate traffic
    if (config.trafficPattern == "uniform") {
        GenerateUniformTraffic(servers, config.numServersPerRack);
    } else if (config.trafficPattern == "hotspot") {
        GenerateHotspotTraffic(servers, config.numServersPerRack);
    }
    
    // Flow monitoring
    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();
    
    // Run simulation
    NS_LOG_INFO("Running simulation for " << config.simTime << " seconds...");
    Simulator::Stop(Seconds(config.simTime));
    Simulator::Run();
    
    // Collect metrics
    CollectMetrics(monitor, flowHelper, metrics);
    
    // Display results
    NS_LOG_INFO("===========================================");
    NS_LOG_INFO("RESULTS - Traffic Pattern: " << config.trafficPattern);
    NS_LOG_INFO("===========================================");
    NS_LOG_INFO("Wireless Backup: " << (config.enableWireless ? "ENABLED" : "DISABLED"));
    NS_LOG_INFO("Average Throughput: " << metrics.avgThroughput << " Mbps");
    NS_LOG_INFO("Average Latency: " << metrics.avgLatency << " ms");
    NS_LOG_INFO("Dropped Packets: " << metrics.droppedPackets);
    
    if (config.enableWireless) {
        NS_LOG_INFO("");
        NS_LOG_INFO("Hybrid Architecture: ToR 2-3 have wireless backup paths");
        NS_LOG_INFO("Wireless neighborways provide " << config.wirelessDataRate << " additional capacity");
    }
    
    Simulator::Destroy();
    return 0;
}
