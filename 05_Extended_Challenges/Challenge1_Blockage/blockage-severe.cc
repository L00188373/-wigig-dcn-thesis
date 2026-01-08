#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/error-model.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("BlockageSevere");

Ptr<RateErrorModel> g_errorModel;

int main(int argc, char *argv[]) {
    bool enableBlockage = true;
    bool enableRedundancy = false;
    
    CommandLine cmd;
    cmd.AddValue("blockage", "Enable blockage", enableBlockage);
    cmd.AddValue("redundancy", "Enable redundancy", enableRedundancy);
    cmd.Parse(argc, argv);
    
    LogComponentEnable("BlockageSevere", LOG_LEVEL_INFO);
    
    // Create simple topology
    NodeContainer nodes;
    nodes.Create(3);  // Simplified: Source -> Link -> Destination
    
    InternetStackHelper internet;
    internet.Install(nodes);
    
    // Main path: Node 0 -> Node 1 -> Node 2
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("1000Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("1ms"));
    
    NetDeviceContainer link01 = p2p.Install(nodes.Get(0), nodes.Get(1));
    NetDeviceContainer link12 = p2p.Install(nodes.Get(1), nodes.Get(2));
    
    // Redundant direct path if enabled: Node 0 -> Node 2
    NetDeviceContainer link02;
    if (enableRedundancy) {
        link02 = p2p.Install(nodes.Get(0), nodes.Get(2));
        NS_LOG_INFO("Redundant path created: Node0 -> Node2 (direct)");
    }
    
    // Assign IPs
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.252");
    address.Assign(link01);
    address.SetBase("10.1.2.0", "255.255.255.252");
    address.Assign(link12);
    if (enableRedundancy) {
        address.SetBase("10.1.3.0", "255.255.255.252");
        address.Assign(link02);
    }
    
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    
    // Create error model for blockage
    g_errorModel = CreateObject<RateErrorModel>();
    g_errorModel->SetRate(0.0);  // Start with no errors
    
    // Apply to the critical link (1->2)
    link12.Get(0)->SetAttribute("ReceiveErrorModel", PointerValue(g_errorModel));
    link12.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(g_errorModel));
    
    // Traffic: Node 0 -> Node 2
    uint16_t port = 9;
    
    OnOffHelper onoff("ns3::TcpSocketFactory",
                     InetSocketAddress("10.1.2.2", port));
    onoff.SetAttribute("DataRate", DataRateValue(DataRate("500Mbps")));
    onoff.SetAttribute("PacketSize", UintegerValue(1024));
    onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    
    ApplicationContainer app = onoff.Install(nodes.Get(0));
    app.Start(Seconds(1));
    app.Stop(Seconds(29));
    
    PacketSinkHelper sink("ns3::TcpSocketFactory",
                         InetSocketAddress(Ipv4Address::GetAny(), port));
    app = sink.Install(nodes.Get(2));
    app.Start(Seconds(0));
    app.Stop(Seconds(30));
    
    // Schedule SEVERE blockage
    if (enableBlockage) {
        Simulator::Schedule(Seconds(10.0), []() {
            NS_LOG_INFO("*** BLOCKAGE: 100% packet loss on link 1->2 ***");
            g_errorModel->SetRate(1.0);  // 100% packet loss
        });
        
        Simulator::Schedule(Seconds(15.0), []() {
            NS_LOG_INFO("*** BLOCKAGE CLEARED ***");
            g_errorModel->SetRate(0.0);  // Normal operation
        });
    }
    
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();
    
    Simulator::Stop(Seconds(30));
    Simulator::Run();
    
    monitor->CheckForLostPackets();
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();
    
    double totalThroughput = 0;
    uint32_t totalLost = 0;
    
    for (auto i = stats.begin(); i != stats.end(); ++i) {
        totalLost += i->second.lostPackets;
        double duration = (i->second.timeLastRxPacket - 
                          i->second.timeFirstTxPacket).GetSeconds();
        if (duration > 0) {
            totalThroughput = (i->second.rxBytes * 8.0) / duration / 1e6;
        }
    }
    
    NS_LOG_INFO("\n=== RESULTS ===");
    NS_LOG_INFO("Blockage: " << (enableBlockage ? "YES" : "NO"));
    NS_LOG_INFO("Redundancy: " << (enableRedundancy ? "YES" : "NO"));
    NS_LOG_INFO("Throughput: " << totalThroughput << " Mbps");
    NS_LOG_INFO("Packets Lost: " << totalLost);
    
    Simulator::Destroy();
    return 0;
}
