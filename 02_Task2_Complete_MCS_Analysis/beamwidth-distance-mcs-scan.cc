/*
 * Comprehensive Beamwidth vs Distance vs MCS Analysis
 * Based on working dcn-wigig-mcs-test.cc
 * Modified to test all combinations and save results
 * 
 * Author: Anthony Malone (L00188373)
 * Supervisor: Dr. Saim Ghafoor
 * Date: November 2024
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
#include <fstream>

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE("ComprehensiveMcsAnalysis");

// Global results file
ofstream g_resultsFile;

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

double RunSingleTest(uint32_t sectors, double distance, uint32_t mcs, double simulationTime)
{
    // Configuration
    uint32_t payloadSize = 1472;
    string msduAggSize = "7935";
    string mpduAggSize = "262143";
    string queueSize = "4000p";
    uint32_t channel = 2;
    bool enableRts = false;
    uint32_t rtsThreshold = 0;
    string wigigErrorModel = "src/wigig/model/reference/ErrorModel/LookupTable_1458.txt";
    
    ValidateFrameAggregationAttributes(msduAggSize, mpduAggSize);
    ConfigureRtsCtsAndFragmenatation(enableRts, rtsThreshold);
    ChangeQueueSize(queueSize);
    
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
                      "Sectors", UintegerValue(sectors));
    
    wigigMac.SetType("ns3::AdhocWigigMac",
                     "BE_MaxAmpduSize", StringValue(mpduAggSize),
                     "BE_MaxAmsduSize", StringValue(msduAggSize));
    
    // Install devices separately
    NetDeviceContainer device1 = wigig.Install(wigigPhy, wigigMac, node1);
    NetDeviceContainer device2 = wigig.Install(wigigPhy, wigigMac, node2);
    
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
    
    // Flow monitor
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();
    
    // Run simulation
    Simulator::Stop(Seconds(simulationTime));
    Simulator::Run();
    
    // Get results
    double throughput = 0;
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();
    
    for (auto const &flow : stats) {
        throughput = flow.second.rxBytes * 8.0 / simulationTime / 1e6;  // Mbps
    }
    
    Simulator::Destroy();
    
    return throughput;
}

int main(int argc, char *argv[])
{
    // Test parameters
    vector<uint32_t> sectorsList = {8, 16, 32};
    vector<double> distancesList = {1.0, 2.0, 4.0, 7.0, 10.0};
    vector<uint32_t> mcsList = {5, 9, 12, 15, 18, 21, 24};  // Valid MCS range: 0-24
    
    double testDuration = 5.0;  // 5 seconds per test
    
    // Open results file
    g_resultsFile.open("comprehensive_mcs_analysis_results.csv");
    g_resultsFile << "Sectors,Beamwidth_deg,Distance_m,MCS,Throughput_Mbps\n";
    
    cout << "═══════════════════════════════════════════════════════════════════════════════\n";
    cout << "COMPREHENSIVE BEAMWIDTH vs DISTANCE vs MCS ANALYSIS\n";
    cout << "═══════════════════════════════════════════════════════════════════════════════\n";
    cout << "Student: Anthony Malone (L00188373)\n";
    cout << "Supervisor: Dr. Saim Ghafoor\n\n";
    cout << "Test Matrix:\n";
    cout << "  Beamwidths: " << sectorsList.size() << " configurations\n";
    cout << "  Distances:  " << distancesList.size() << " points\n";
    cout << "  MCS Levels: " << mcsList.size() << " levels\n";
    cout << "  Total Sims: " << (sectorsList.size() * distancesList.size() * mcsList.size()) << "\n";
    cout << "  Duration:   ~" << (sectorsList.size() * distancesList.size() * mcsList.size() * 2) / 60 << " minutes\n";
    cout << "═══════════════════════════════════════════════════════════════════════════════\n\n";
    
    uint32_t totalTests = sectorsList.size() * distancesList.size() * mcsList.size();
    uint32_t currentTest = 0;
    
    auto startTime = chrono::steady_clock::now();
    
    // Run all combinations
    for (uint32_t sectors : sectorsList) {
        double beamwidth = 360.0 / sectors;
        
        for (double distance : distancesList) {
            for (uint32_t mcs : mcsList) {
                currentTest++;
                
                // Progress indicator
                cout << "\r[" << currentTest << "/" << totalTests << "] ";
                cout << "Testing: " << sectors << " sectors (" << beamwidth << "°), ";
                cout << distance << "m, MCS " << mcs << "     " << flush;
                
                try {
                    // Run test
                    double throughput = RunSingleTest(sectors, distance, mcs, testDuration);
                    
                    // Write results
                    g_resultsFile << sectors << ","
                                << fixed << setprecision(2) << beamwidth << ","
                                << distance << ","
                                << mcs << ","
                                << setprecision(2) << throughput << "\n";
                    g_resultsFile.flush();
                }
                catch (exception& e) {
                    cout << "\n  ERROR: " << e.what() << "\n";
                    g_resultsFile << sectors << "," << beamwidth << "," << distance << "," 
                                << mcs << ",0.00\n";
                    g_resultsFile.flush();
                }
                
                // Show milestone results every 10 tests
                if (currentTest % 10 == 0) {
                    auto currentTime = chrono::steady_clock::now();
                    auto elapsed = chrono::duration_cast<chrono::minutes>(
                        currentTime - startTime).count();
                    auto estimated = (elapsed * totalTests) / currentTest;
                    auto remaining = estimated - elapsed;
                    
                    cout << "\n  ├─ Progress: " << (100 * currentTest / totalTests) << "%";
                    cout << " | Elapsed: " << elapsed << "m";
                    cout << " | Remaining: ~" << remaining << "m\n";
                }
            }
        }
    }
    
    g_resultsFile.close();
    
    auto endTime = chrono::steady_clock::now();
    auto totalTime = chrono::duration_cast<chrono::minutes>(
        endTime - startTime).count();
    
    cout << "\n\n═══════════════════════════════════════════════════════════════════════════════\n";
    cout << "SIMULATION COMPLETE!\n";
    cout << "═══════════════════════════════════════════════════════════════════════════════\n";
    cout << "Total simulations: " << totalTests << "\n";
    cout << "Total time:        " << totalTime << " minutes\n";
    cout << "Results saved to:  comprehensive_mcs_analysis_results.csv\n\n";
    cout << "Next steps:\n";
    cout << "  1. Run Python analysis script to create visualizations\n";
    cout << "  2. Review results for optimal MCS selection\n";
    cout << "  3. Prepare graphs for thesis and Dr. Ghafoor meeting\n";
    cout << "═══════════════════════════════════════════════════════════════════════════════\n";
    
    return 0;
}
