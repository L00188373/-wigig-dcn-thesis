# Task 4: Hybrid Architecture Analysis - Progress Report

**Student:** Anthony Malone  
**Date:** November 19, 2025  
**Status:** Baseline Implementation Complete (40% overall thesis progress)

---

## Executive Summary

I have successfully implemented the simulation framework for Task 4 (Hybrid Architecture Analysis) and obtained baseline results that clearly demonstrate the hotspot problem in data center networks. The results validate the need for hybrid wired/wireless architecture using 60 GHz WiGig technology.

---

## What I've Built

### Network Topology
- **4 ToR (Top-of-Rack) switches**
  - ToR 0-1: Wired-only (control group)
  - ToR 2-3: Designated for hybrid wired/WiGig implementation
- **2 Spine switches** (core layer)
- **80 servers total** (20 per rack)
- **10 Gbps wired links** throughout the network

### Traffic Scenarios Implemented

1. **Uniform Traffic Pattern**
   - All servers communicate evenly across the network
   - Represents normal, balanced operation

2. **Hotspot Traffic Pattern**
   - 80% of traffic concentrates at ToR 2 (one of the hybrid-designated ToRs)
   - Simulates real-world scenarios like:
     - Popular microservice receiving high request volume
     - MapReduce shuffle phase concentrating data at specific nodes
     - Database server under heavy query load

---

## Baseline Results

### Uniform Traffic (Normal Operation)
```
Average Throughput:  127.157 Mbps
Average Latency:     2.6412 ms
Dropped Packets:     12
```

### Hotspot Traffic (Congestion Scenario)
```
Average Throughput:  74.3 Mbps
Average Latency:     2.6522 ms
Dropped Packets:     11,414
```

### Impact Analysis

| Metric | Uniform | Hotspot | Change |
|--------|---------|---------|--------|
| **Throughput** | 127.2 Mbps | 74.3 Mbps | **-41.5% ↓** |
| **Latency** | 2.64 ms | 2.65 ms | Minimal |
| **Packet Drops** | 12 | 11,414 | **+95,016% ↑** |

---

## What This Means

### The Problem is Clear

When traffic concentrates on one ToR switch (hotspot scenario), the wired links become severely congested:

1. **Massive Packet Loss**: Dropped packets increased from 12 to 11,414
   - Buffer overflow at congested switches
   - TCP retransmissions further worsen congestion
   
2. **Throughput Degradation**: 41.5% reduction in effective bandwidth
   - Applications experience slow response times
   - User-facing services degraded

3. **Bottleneck Location**: The two 10 Gbps wired links between ToR 2 and the Spine switches cannot handle the concentrated traffic load

### Why This Happens

In the hotspot scenario:
- 64 servers (from ToRs 0, 1, 3) → all sending to 20 servers (on ToR 2)
- All traffic must flow through: Source → ToR → **Spine → ToR 2** → Destination
- The wired links to ToR 2 become **severe bottlenecks**
- Queue buffers fill up → packets drop → TCP slows down → throughput collapses

---

## Connection to Research Literature

My implementation aligns with the approach described in:

**"Relieving Hotspots in Data Center Networks with Wireless Neighborways"**  
*Shan et al., IEEE Globecom 2014*

### Key Concepts from the Paper

1. **Wireless Neighborways**: Each ToR equipped with 60 GHz wireless links to neighboring ToRs

2. **Cache Occupancy Rate (β)**: Detection metric for hotspots
   - When β > 90%, switch is considered a hotspot
   - My simulation shows ToR 2 is clearly a hotspot (massive packet drops)

3. **Traffic Offloading Strategy**:
   - Congested ToR offloads traffic via wireless links to less-busy neighboring ToRs
   - Neighboring ToRs route traffic through **under-utilized wired paths**
   - Traffic converges back to destination via wireless links

4. **Results from Paper**:
   - Hotspots reduced from ~140 to ~30 switches
   - Cache occupancy balanced across the DCN fabric
   - Network utilization improved significantly

---

## Next Steps: WiGig Implementation

### Phase 1: Add WiGig Infrastructure (Estimated: 1-2 weeks)

1. **Install 60 GHz Wireless Devices**
   - Add WiGig NetDevices to ToR 2 and ToR 3
   - Configure directional antennas (beamwidth: 30-60°)
   - Model short-range, high-bandwidth links (4+ Gbps)

2. **Create Wireless Neighborways**
   - ToR 2 ↔ ToR 1 (wireless backup)
   - ToR 2 ↔ ToR 3 (wireless backup)
   - ToR 3 ↔ ToR 1 (wireless backup)

### Phase 2: Implement Congestion Detection

1. **Monitor Queue Occupancy** at each ToR
2. **Define Threshold**: When dropped packets > X, activate wireless failover
3. **Track Metrics**: Monitor β (cache occupancy rate) per ToR

### Phase 3: Traffic Offloading Logic

1. **Detect Hotspot**: When ToR 2 experiences high congestion
2. **Activate Wireless Links**: Offload traffic to ToR 1 and ToR 3
3. **Load Balancing**: Distribute offloaded traffic proportionally based on neighbor capacity
4. **Route Through Alternate Paths**: Use under-utilized wired links in the fabric

### Expected Outcomes

Based on the research literature, I expect the hybrid implementation to show:

- **Reduced packet drops**: From 11,414 to < 1,000
- **Improved throughput**: From 74.3 Mbps back toward 127 Mbps
- **Balanced load**: Distribute congestion across multiple paths
- **Graceful degradation**: Even under hotspot conditions, maintain acceptable performance

---

## Timeline

- **Week 1-2**: Implement WiGig devices and wireless neighborways
- **Week 3**: Add congestion detection and failover logic
- **Week 4**: Run comprehensive experiments and collect data
- **Week 5**: Analyze results and compare against baseline
- **Week 6**: Write up Task 4 section of thesis

---

## Conclusion

The baseline results clearly demonstrate the hotspot problem in data center networks and validate the need for hybrid wired/wireless architecture. The simulation framework is working correctly and ready for WiGig implementation. The next phase will show how 60 GHz wireless technology can effectively relieve congestion and maintain high performance even under unbalanced traffic conditions.

---

## Files in This Task

- `Code/hybrid-dcn-task4.cc` - Simulation source code
- `Results/task4_uniform_baseline.txt` - Uniform traffic results
- `Results/task4_hotspot_baseline.txt` - Hotspot traffic results  
- `Documentation/Task4_Progress_Report.md` - This document
- `Documentation/Relieving_hotspots_paper.pdf` - Reference paper (to be added)
