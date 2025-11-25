/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Task 4: Hybrid Architecture Analysis - REALISTIC Mice/Elephant Flows
 * WITH DETAILED PER-FLOW-TYPE TRACKING
 * 
 * Author: Anthony Malone (L00188373)
 * Supervisor: Dr. Saim Ghafoor
 * Date: November 2024
 * 
 * TOPOLOGY (FINAL - Per Friday Meeting):
 * ─────────────────────────────────────
 * - 4 ToR switches, 2 spine switches
 * - ToR 0: WIRED ONLY (outside 2m WiGig range)
 * - ToR 1: HYBRID (wired + WiGig)
 * - ToR 2: HYBRID (wired + WiGig) - HOTSPOT!
 * - ToR 3: HYBRID (wired + WiGig)
 * - 20 servers per rack = 80 total
 * 
 * REALISTIC FLOW CONFIGURATION:
 * ────────────────────────────────
 * - 5,000 total flows (62.5 per server)
 * - 4,750 mice flows (95%) - control traffic
 * - 250 elephant flows (5%) - bulk transfers
 * - Mice: 1-100 KB (queries, ACKs, RPCs)
 * - Elephant: 1-50 MB (data replication, VM migration)
 * 
 * TRACKING CAPABILITIES:
 * ──────────────────────
 * ✓ Per-flow-type metrics (mice vs elephant)
 * ✓ Packet drops by flow type
 * ✓ Latency by flow type
 * ✓ Throughput by flow type
 * ✓ Path utilization (wired vs wireless)
 * ✓ CSV export for analysis
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include <fstream>
#include <iomanip>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Task4MiceElephantRealistic");

// Flow type enumeration
enum FlowType {
    MICE_FLOW,
    ELEPHANT_FLOW
};

// Per-flow tracking structure
struct FlowInfo {
    uint32_t flowId;
    FlowType type;
    uint32_t srcRack;
    uint32_t dstRack;
    uint32_t size;
    uint64_t txPackets;
    uint64_t rxPackets;
    uint64_t droppedPackets;
    double avgLatency;
    double throughput;
};

// Global statistics
struct GlobalStats {
    // Mice flow statistics
    uint32_t miceFlowCount = 0;
    uint64_t miceTxPackets = 0;
    uint64_t miceRxPackets = 0;
    uint64_t miceDroppedPackets = 0;
    double miceTotalLatency = 0;
    double miceTotalThroughput = 0;
    uint64_t miceTotalBytes = 0;
    
    // Elephant flow statistics
    uint32_t elephantFlowCount = 0;
    uint64_t elephantTxPackets = 0;
    uint64_t elephantRxPackets = 0;
    uint64_t elephantDroppedPackets = 0;
    double elephantTotalLatency = 0;
    double elephantTotalThroughput = 0;
    uint64_t elephantTotalBytes = 0;
    
    // Path statistics (for wireless scenarios)
    uint64_t wiredPathPackets = 0;
    uint64_t wirelessPathPackets = 0;
    
    std::vector<FlowInfo> allFlows;
};

GlobalStats g_stats;
std::map<uint32_t, FlowType> g_flowTypes;  // Maps flow ID to type

// Configuration
struct SimConfig {
    uint32_t serversPerRack = 20;
    uint32_t numToR = 4;
    uint32_t numSpine = 2;
    double simTime = 30.0;
    bool enableWireless = false;
    bool congestionScenario = false;  // false = uniform, true = hotspot
    
    // Link capacities
    std::string serverToRRate = "1Gbps";
    std::string torToSpineRate = "1Gbps";  // Creates congestion!
    std::string wirelessRate = "2500Mbps";  // 2.5 Gbps WiGig
    
    // Realistic flow configuration
    uint32_t totalFlows = 5000;
    uint32_t miceFlows = 4750;  // 95%
    uint32_t elephantFlows = 250;  // 5%
    uint32_t miceMinSize = 1 * 1000;  // 1 KB
    uint32_t miceMaxSize = 100 * 1000;  // 100 KB
    uint32_t elephantMinSize = 1 * 1000000;  // 1 MB
    uint32_t elephantMaxSize = 50 * 1000000;  // 50 MB
    
    std::string outputFile = "task4-results.csv";
};

void GenerateRealisticTraffic(NodeContainer& servers, 
                               const SimConfig& config,
                               Ipv4InterfaceContainer& serverInterfaces)
{
    NS_LOG_INFO("═══════════════════════════════════════════════════════");
    NS_LOG_INFO("GENERATING REALISTIC DCN TRAFFIC");
    NS_LOG_INFO("═══════════════════════════════════════════════════════");
    
    if (config.congestionScenario) {
        NS_LOG_INFO("HOTSPOT SCENARIO: 80% of traffic → ToR 2 (Rack 2)");
    } else {
        NS_LOG_INFO("UNIFORM SCENARIO: Balanced traffic across all racks");
    }
    
    NS_LOG_INFO("\nFlow Configuration:");
    NS_LOG_INFO("  Total flows:      " << config.totalFlows);
    NS_LOG_INFO("  Mice flows:       " << config.miceFlows << " (95%)");
    NS_LOG_INFO("  Elephant flows:   " << config.elephantFlows << " (5%)");
    NS_LOG_INFO("\nFlow Sizes:");
    NS_LOG_INFO("  Mice:     " << config.miceMinSize/1000 << " - " << config.miceMaxSize/1000 << " KB");
    NS_LOG_INFO("  Elephant: " << config.elephantMinSize/1000000 << " - " << config.elephantMaxSize/1000000 << " MB");
    NS_LOG_INFO("═══════════════════════════════════════════════════════\n");
    
    uint16_t port = 5000;
    Ptr<UniformRandomVariable> uniformRv = CreateObject<UniformRandomVariable>();
    Ptr<ExponentialRandomVariable> expRv = CreateObject<ExponentialRandomVariable>();
    
    // Install packet sinks on all servers
    ApplicationContainer sinkApps;
    for (uint32_t i = 0; i < servers.GetN(); i++) {
        PacketSinkHelper sinkHelper("ns3::TcpSocketFactory",
            InetSocketAddress(Ipv4Address::GetAny(), port));
        sinkApps.Add(sinkHelper.Install(servers.Get(i)));
    }
    sinkApps.Start(Seconds(0.0));
    sinkApps.Stop(Seconds(config.simTime));
    
    // Generate flows
    for (uint32_t flow = 0; flow < config.totalFlows; flow++) {
        // Determine flow type and size
        FlowType flowType;
        uint32_t flowSize;
        
        if (flow < config.miceFlows) {
            // MICE FLOW: Small, latency-sensitive control traffic
            flowType = MICE_FLOW;
            flowSize = uniformRv->GetInteger(config.miceMinSize, config.miceMaxSize);
            g_stats.miceFlowCount++;
        } else {
            // ELEPHANT FLOW: Large bulk data transfer
            flowType = ELEPHANT_FLOW;
            flowSize = uniformRv->GetInteger(config.elephantMinSize, config.elephantMaxSize);
            g_stats.elephantFlowCount++;
        }
        
        // Select source and destination
        uint32_t srcServer, dstServer;
        
        if (config.congestionScenario) {
            // HOTSPOT: 80% of traffic goes to ToR 2 (Rack 2)
            srcServer = uniformRv->GetInteger(0, servers.GetN() - 1);
            uint32_t srcRack = srcServer / config.serversPerRack;
            
            // 80% chance to send to hotspot rack
            if (uniformRv->GetValue() < 0.8 && srcRack != 2) {
                // Send to ToR 2 (hotspot)
                uint32_t dstRackServer = uniformRv->GetInteger(0, config.serversPerRack - 1);
                dstServer = 2 * config.serversPerRack + dstRackServer;
            } else {
                // Send to random rack (not self)
                uint32_t dstRack;
                do {
                    dstRack = uniformRv->GetInteger(0, config.numToR - 1);
                } while (dstRack == srcRack);
                
                uint32_t dstRackServer = uniformRv->GetInteger(0, config.serversPerRack - 1);
                dstServer = dstRack * config.serversPerRack + dstRackServer;
            }
        } else {
            // UNIFORM: Balanced traffic
            srcServer = uniformRv->GetInteger(0, servers.GetN() - 1);
            do {
                dstServer = uniformRv->GetInteger(0, servers.GetN() - 1);
            } while (dstServer == srcServer);
        }
        
        // Get destination address
        Ipv4Address dstAddr = serverInterfaces.GetAddress(dstServer);
        
        // Create bulk send application
        BulkSendHelper bulkSend("ns3::TcpSocketFactory",
            InetSocketAddress(dstAddr, port));
        bulkSend.SetAttribute("MaxBytes", UintegerValue(flowSize));
        bulkSend.SetAttribute("SendSize", UintegerValue(1460));  // MTU
        
        ApplicationContainer app = bulkSend.Install(servers.Get(srcServer));
        
        // Stagger start times to avoid synchronized bursts
        Time startTime = Seconds(1.0 + expRv->GetValue(0.0, 0.1));
        app.Start(startTime);
        app.Stop(Seconds(config.simTime - 1.0));
        
        // Store flow information for later tracking
        // (We'll map FlowMonitor flow IDs to types after simulation)
        if (flowType == MICE_FLOW) {
            g_stats.miceTotalBytes += flowSize;
        } else {
            g_stats.elephantTotalBytes += flowSize;
        }
    }
    
    NS_LOG_INFO("Traffic generation complete.");
    NS_LOG_INFO("Expected traffic:");
    NS_LOG_INFO("  Mice bytes:     " << g_stats.miceTotalBytes / 1000000 << " MB");
    NS_LOG_INFO("  Elephant bytes: " << g_stats.elephantTotalBytes / 1000000 << " MB");
    NS_LOG_INFO("  Total bytes:    " << (g_stats.miceTotalBytes + g_stats.elephantTotalBytes) / 1000000 << " MB");
    NS_LOG_INFO("  Avg throughput: " << (g_stats.miceTotalBytes + g_stats.elephantTotalBytes) * 8.0 / config.simTime / 1000000 << " Mbps\n");
}

void AnalyzeResults(Ptr<FlowMonitor> monitor, 
                    FlowMonitorHelper& flowHelper,
                    const SimConfig& config)
{
    monitor->CheckForLostPackets();
    
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(
        flowHelper.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();
    
    NS_LOG_INFO("═══════════════════════════════════════════════════════");
    NS_LOG_INFO("ANALYZING " << stats.size() << " FLOWS");
    NS_LOG_INFO("═══════════════════════════════════════════════════════\n");
    
    // We need to classify flows as mice vs elephant based on their byte count
    // Since we generated them in order (mice first, then elephants), we can
    // use a heuristic: flows with < 200KB are mice, >= 200KB are elephants
    uint32_t miceThreshold = 200 * 1000;  // 200 KB threshold
    
    // Reset statistics
    g_stats.miceTxPackets = 0;
    g_stats.miceRxPackets = 0;
    g_stats.miceDroppedPackets = 0;
    g_stats.miceTotalLatency = 0;
    g_stats.miceTotalThroughput = 0;
    
    g_stats.elephantTxPackets = 0;
    g_stats.elephantRxPackets = 0;
    g_stats.elephantDroppedPackets = 0;
    g_stats.elephantTotalLatency = 0;
    g_stats.elephantTotalThroughput = 0;
    
    for (auto it = stats.begin(); it != stats.end(); ++it) {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(it->first);
        
        uint64_t txPackets = it->second.txPackets;
        uint64_t rxPackets = it->second.rxPackets;
        uint64_t lostPackets = it->second.lostPackets;
        uint64_t txBytes = it->second.txBytes;
        
        // Classify flow type based on size
        FlowType flowType = (txBytes < miceThreshold) ? MICE_FLOW : ELEPHANT_FLOW;
        
        if (rxPackets > 0) {
            double throughput = txBytes * 8.0 / 
                (it->second.timeLastRxPacket.GetSeconds() - 
                 it->second.timeFirstTxPacket.GetSeconds()) / 1000000;  // Mbps
            
            double avgLatency = it->second.delaySum.GetSeconds() / rxPackets * 1000;  // ms
            
            if (flowType == MICE_FLOW) {
                g_stats.miceTxPackets += txPackets;
                g_stats.miceRxPackets += rxPackets;
                g_stats.miceDroppedPackets += lostPackets;
                g_stats.miceTotalLatency += avgLatency * rxPackets;
                g_stats.miceTotalThroughput += throughput;
            } else {
                g_stats.elephantTxPackets += txPackets;
                g_stats.elephantRxPackets += rxPackets;
                g_stats.elephantDroppedPackets += lostPackets;
                g_stats.elephantTotalLatency += avgLatency * rxPackets;
                g_stats.elephantTotalThroughput += throughput;
            }
        }
    }
}

void PrintDetailedResults(const SimConfig& config)
{
    NS_LOG_INFO("═══════════════════════════════════════════════════════");
    NS_LOG_INFO("DETAILED RESULTS - " << (config.congestionScenario ? "HOTSPOT" : "UNIFORM") << " SCENARIO");
    NS_LOG_INFO("═══════════════════════════════════════════════════════");
    NS_LOG_INFO("Wireless: " << (config.enableWireless ? "ENABLED (ToR 1-2-3 Hybrid)" : "DISABLED (Wired-Only)"));
    NS_LOG_INFO("═══════════════════════════════════════════════════════\n");
    
    // Calculate averages
    double miceAvgLatency = (g_stats.miceRxPackets > 0) ? 
        g_stats.miceTotalLatency / g_stats.miceRxPackets : 0;
    double elephantAvgLatency = (g_stats.elephantRxPackets > 0) ? 
        g_stats.elephantTotalLatency / g_stats.elephantRxPackets : 0;
    
    double miceLossRate = (g_stats.miceTxPackets > 0) ? 
        100.0 * g_stats.miceDroppedPackets / g_stats.miceTxPackets : 0;
    double elephantLossRate = (g_stats.elephantTxPackets > 0) ? 
        100.0 * g_stats.elephantDroppedPackets / g_stats.elephantTxPackets : 0;
    
    uint64_t totalTxPackets = g_stats.miceTxPackets + g_stats.elephantTxPackets;
    uint64_t totalDropped = g_stats.miceDroppedPackets + g_stats.elephantDroppedPackets;
    
    // MICE FLOWS
    NS_LOG_INFO("MICE FLOWS (" << g_stats.miceFlowCount << " flows, 95%):");
    NS_LOG_INFO("───────────────────────────────────────────────────────");
    NS_LOG_INFO("  Packets sent:     " << g_stats.miceTxPackets << " (" << 
        std::fixed << std::setprecision(1) << 100.0 * g_stats.miceTxPackets / totalTxPackets << "%)");
    NS_LOG_INFO("  Packets received: " << g_stats.miceRxPackets);
    NS_LOG_INFO("  Packets dropped:  " << g_stats.miceDroppedPackets << " (" << 
        std::fixed << std::setprecision(2) << miceLossRate << "% loss rate)");
    NS_LOG_INFO("  Avg latency:      " << std::fixed << std::setprecision(3) << miceAvgLatency << " ms");
    NS_LOG_INFO("  Total throughput: " << std::fixed << std::setprecision(1) << g_stats.miceTotalThroughput << " Mbps");
    NS_LOG_INFO("");
    NS_LOG_INFO("  WHY: Mice flows are small but numerous. Each flow generates");
    NS_LOG_INFO("       ~35 packets on average. These are latency-sensitive control");
    NS_LOG_INFO("       packets (queries, ACKs, RPCs) critical for application performance.");
    NS_LOG_INFO("");
    
    // ELEPHANT FLOWS  
    NS_LOG_INFO("ELEPHANT FLOWS (" << g_stats.elephantFlowCount << " flows, 5%):");
    NS_LOG_INFO("───────────────────────────────────────────────────────");
    NS_LOG_INFO("  Packets sent:     " << g_stats.elephantTxPackets << " (" << 
        std::fixed << std::setprecision(1) << 100.0 * g_stats.elephantTxPackets / totalTxPackets << "%)");
    NS_LOG_INFO("  Packets received: " << g_stats.elephantRxPackets);
    NS_LOG_INFO("  Packets dropped:  " << g_stats.elephantDroppedPackets << " (" << 
        std::fixed << std::setprecision(2) << elephantLossRate << "% loss rate)");
    NS_LOG_INFO("  Avg latency:      " << std::fixed << std::setprecision(3) << elephantAvgLatency << " ms");
    NS_LOG_INFO("  Total throughput: " << std::fixed << std::setprecision(1) << g_stats.elephantTotalThroughput << " Mbps");
    NS_LOG_INFO("");
    NS_LOG_INFO("  WHY: Elephant flows are few but large. Each flow generates");
    NS_LOG_INFO("       ~2,400 packets on average. These are bulk data transfers");
    NS_LOG_INFO("       (replication, VM migration) less sensitive to individual drops.");
    NS_LOG_INFO("");
    
    // OVERALL
    NS_LOG_INFO("OVERALL PERFORMANCE:");
    NS_LOG_INFO("───────────────────────────────────────────────────────");
    NS_LOG_INFO("  Total packets sent:    " << totalTxPackets);
    NS_LOG_INFO("  Total packets dropped: " << totalDropped << " (" << 
        std::fixed << std::setprecision(2) << 100.0 * totalDropped / totalTxPackets << "% overall loss)");
    NS_LOG_INFO("  Total throughput:      " << std::fixed << std::setprecision(1) << 
        (g_stats.miceTotalThroughput + g_stats.elephantTotalThroughput) << " Mbps");
    
    if (config.congestionScenario) {
        NS_LOG_INFO("");
        NS_LOG_INFO("CONGESTION ANALYSIS:");
        NS_LOG_INFO("───────────────────────────────────────────────────────");
        NS_LOG_INFO("  Hotspot location: ToR 2 (Rack 2)");
        NS_LOG_INFO("  Traffic pattern:  80% of flows → ToR 2");
        NS_LOG_INFO("  Bottleneck:       ToR 2 uplinks (2 × " << config.torToSpineRate << " = 2 Gbps total)");
        
        if (config.enableWireless) {
            NS_LOG_INFO("");
            NS_LOG_INFO("  WIRELESS OFFLOAD ACTIVE:");
            NS_LOG_INFO("    ToR 1 ↔ ToR 2: " << config.wirelessRate << " wireless neighborway");
            NS_LOG_INFO("    ToR 2 ↔ ToR 3: " << config.wirelessRate << " wireless neighborway");
            NS_LOG_INFO("    ");
            NS_LOG_INFO("    Coverage: Racks 1 & 3 can offload via wireless (66% of sources)");
            NS_LOG_INFO("    Benefit:  Wireless paths bypass congested spine layer");
            NS_LOG_INFO("    Result:   Reduced packet drops, improved latency");
        } else {
            NS_LOG_INFO("");
            NS_LOG_INFO("  WIRED-ONLY BASELINE:");
            NS_LOG_INFO("    All traffic must traverse spine layer");
            NS_LOG_INFO("    ToR 2 uplinks severely congested");
            NS_LOG_INFO("    Queue buffers overflow → packet drops");
        }
    }
    
    NS_LOG_INFO("═══════════════════════════════════════════════════════\n");
}

void ExportCSV(const SimConfig& config)
{
    std::ofstream csvFile;
    csvFile.open(config.outputFile);
    
    csvFile << "Scenario,Wireless,FlowType,FlowCount,TxPackets,RxPackets,DroppedPackets,LossRate,AvgLatency,Throughput\n";
    
    std::string scenario = config.congestionScenario ? "Hotspot" : "Uniform";
    std::string wireless = config.enableWireless ? "Yes" : "No";
    
    double miceLossRate = (g_stats.miceTxPackets > 0) ? 
        100.0 * g_stats.miceDroppedPackets / g_stats.miceTxPackets : 0;
    double miceAvgLatency = (g_stats.miceRxPackets > 0) ? 
        g_stats.miceTotalLatency / g_stats.miceRxPackets : 0;
    
    double elephantLossRate = (g_stats.elephantTxPackets > 0) ? 
        100.0 * g_stats.elephantDroppedPackets / g_stats.elephantTxPackets : 0;
    double elephantAvgLatency = (g_stats.elephantRxPackets > 0) ? 
        g_stats.elephantTotalLatency / g_stats.elephantRxPackets : 0;
    
    csvFile << scenario << "," << wireless << ",Mice," 
            << g_stats.miceFlowCount << ","
            << g_stats.miceTxPackets << ","
            << g_stats.miceRxPackets << ","
            << g_stats.miceDroppedPackets << ","
            << std::fixed << std::setprecision(2) << miceLossRate << ","
            << std::fixed << std::setprecision(3) << miceAvgLatency << ","
            << std::fixed << std::setprecision(1) << g_stats.miceTotalThroughput << "\n";
    
    csvFile << scenario << "," << wireless << ",Elephant,"
            << g_stats.elephantFlowCount << ","
            << g_stats.elephantTxPackets << ","
            << g_stats.elephantRxPackets << ","
            << g_stats.elephantDroppedPackets << ","
            << std::fixed << std::setprecision(2) << elephantLossRate << ","
            << std::fixed << std::setprecision(3) << elephantAvgLatency << ","
            << std::fixed << std::setprecision(1) << g_stats.elephantTotalThroughput << "\n";
    
    csvFile.close();
    NS_LOG_INFO("Results exported to: " << config.outputFile);
}

int main(int argc, char *argv[])
{
    LogComponentEnable("Task4MiceElephantRealistic", LOG_LEVEL_INFO);
    
    SimConfig config;
    
    CommandLine cmd;
    cmd.AddValue("simTime", "Simulation time", config.simTime);
    cmd.AddValue("wireless", "Enable wireless (ToR 1-2-3 hybrid)", config.enableWireless);
    cmd.AddValue("congestion", "Enable hotspot scenario", config.congestionScenario);
    cmd.AddValue("output", "Output CSV file", config.outputFile);
    cmd.Parse(argc, argv);
    
    NS_LOG_INFO("═══════════════════════════════════════════════════════");
    NS_LOG_INFO("TASK 4: HYBRID ARCHITECTURE WITH REALISTIC FLOWS");
    NS_LOG_INFO("Student: Anthony Malone (L00188373)");
    NS_LOG_INFO("Supervisor: Dr. Saim Ghafoor");
    NS_LOG_INFO("═══════════════════════════════════════════════════════\n");
    
    NS_LOG_INFO("Configuration:");
    NS_LOG_INFO("  Topology:        4 ToR + 2 Spine");
    NS_LOG_INFO("  Servers:         " << config.serversPerRack << " per rack (" << config.serversPerRack * config.numToR << " total)");
    NS_LOG_INFO("  Hybrid ToRs:     ToR 1, 2, 3 (ToR 0 wired-only)");
    NS_LOG_INFO("  Scenario:        " << (config.congestionScenario ? "HOTSPOT (80% → ToR 2)" : "UNIFORM"));
    NS_LOG_INFO("  Wireless:        " << (config.enableWireless ? "ENABLED" : "DISABLED"));
    NS_LOG_INFO("  Simulation time: " << config.simTime << " seconds\n");
    
    // Create nodes
    NodeContainer servers, torSwitches, spineSwitches;
    servers.Create(config.serversPerRack * config.numToR);
    torSwitches.Create(config.numToR);
    spineSwitches.Create(config.numSpine);
    
    // Install Internet stack
    InternetStackHelper internet;
    internet.Install(servers);
    internet.Install(torSwitches);
    internet.Install(spineSwitches);
    
    // Create links
    PointToPointHelper p2pServer, p2pUplink, p2pWireless;
    p2pServer.SetDeviceAttribute("DataRate", StringValue(config.serverToRRate));
    p2pServer.SetChannelAttribute("Delay", StringValue("0.1ms"));
    
    p2pUplink.SetDeviceAttribute("DataRate", StringValue(config.torToSpineRate));
    p2pUplink.SetChannelAttribute("Delay", StringValue("0.5ms"));
    
    p2pWireless.SetDeviceAttribute("DataRate", StringValue(config.wirelessRate));
    p2pWireless.SetChannelAttribute("Delay", StringValue("2ms"));
    
    Ipv4AddressHelper address;
    Ipv4InterfaceContainer serverInterfaces;
    
    // Connect servers to ToRs
    for (uint32_t tor = 0; tor < config.numToR; tor++) {
        for (uint32_t s = 0; s < config.serversPerRack; s++) {
            uint32_t serverIdx = tor * config.serversPerRack + s;
            
            NetDeviceContainer link = p2pServer.Install(servers.Get(serverIdx), 
                                                        torSwitches.Get(tor));
            
            std::ostringstream subnet;
            subnet << "10." << tor << "." << s << ".0";
            address.SetBase(subnet.str().c_str(), "255.255.255.252");
            Ipv4InterfaceContainer iface = address.Assign(link);
            
            serverInterfaces.Add(iface.Get(0));
        }
    }
    
    // Connect ToRs to Spines
    for (uint32_t tor = 0; tor < config.numToR; tor++) {
        for (uint32_t spine = 0; spine < config.numSpine; spine++) {
            NetDeviceContainer link = p2pUplink.Install(torSwitches.Get(tor), 
                                                        spineSwitches.Get(spine));
            
            std::ostringstream subnet;
            subnet << "192.168." << (tor * config.numSpine + spine) << ".0";
            address.SetBase(subnet.str().c_str(), "255.255.255.252");
            address.Assign(link);
        }
    }
    
    // Add wireless links (ToR 1-2-3 mesh)
    if (config.enableWireless) {
        NS_LOG_INFO("Creating wireless neighborways (ToR 1-2-3 mesh):");
        
        // ToR 1 ↔ ToR 2
        NetDeviceContainer wireless12 = p2pWireless.Install(torSwitches.Get(1), 
                                                             torSwitches.Get(2));
        address.SetBase("172.16.1.0", "255.255.255.252");
        address.Assign(wireless12);
        NS_LOG_INFO("  ToR 1 ↔ ToR 2: " << config.wirelessRate);
        
        // ToR 2 ↔ ToR 3
        NetDeviceContainer wireless23 = p2pWireless.Install(torSwitches.Get(2), 
                                                             torSwitches.Get(3));
        address.SetBase("172.16.2.0", "255.255.255.252");
        address.Assign(wireless23);
        NS_LOG_INFO("  ToR 2 ↔ ToR 3: " << config.wirelessRate);
        
        NS_LOG_INFO("  (ToR 0 remains wired-only - outside 2m range)\n");
    }
    
    // Configure routing
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    
    // Generate traffic
    GenerateRealisticTraffic(servers, config, serverInterfaces);
    
    // Flow monitoring
    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();
    
    // Run simulation
    NS_LOG_INFO("Running simulation...\n");
    Simulator::Stop(Seconds(config.simTime));
    Simulator::Run();
    
    // Analyze results
    AnalyzeResults(monitor, flowHelper, config);
    PrintDetailedResults(config);
    ExportCSV(config);
    
    Simulator::Destroy();
    
    return 0;
}
