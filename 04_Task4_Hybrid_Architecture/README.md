# Task 4: Hybrid Architecture Analysis

**Status:** Baseline Complete - WiGig Implementation Pending  
**Completion:** 25% (Framework done, WiGig implementation next)

## Overview

This task analyzes the benefits of hybrid wired/wireless data center networks using 60 GHz WiGig technology to relieve hotspot congestion. The simulation demonstrates how concentrated traffic creates severe bottlenecks in pure wired networks and establishes the baseline for implementing wireless failover mechanisms.

## Network Architecture
```
┌─────────────────────────────────────────┐
│         Spine Layer (2 switches)        │
│         Spine 0      Spine 1            │
└─────────────┬────────────┬──────────────┘
              │            │
     ┌────────┴────┬───────┴────┬─────────┐
     │             │            │         │
┌────▼───┐   ┌────▼───┐   ┌────▼───┐ ┌──▼─────┐
│ ToR 0  │   │ ToR 1  │   │ ToR 2  │ │ ToR 3  │
│(Wired) │   │(Wired) │   │(Hybrid)│ │(Hybrid)│
└────┬───┘   └────┬───┘   └────┬───┘ └───┬────┘
     │            │            │         │
  20 servers   20 servers   20 servers  20 servers
```

### Configuration
- **ToR 0-1**: Wired-only (control group)
- **ToR 2-3**: Designated for hybrid wired+WiGig
- **Links**: 10 Gbps wired connections
- **Servers**: 20 per rack, 80 total

## Traffic Patterns

### 1. Uniform Traffic
- All servers communicate evenly
- Balanced load distribution
- Baseline performance measurement

### 2. Hotspot Traffic
- **80% of traffic** targets ToR 2
- Simulates real-world congestion:
  - Popular microservice
  - MapReduce shuffle phase
  - Database query concentration

## Key Results

### Baseline Performance (Wired-Only)

| Metric | Uniform | Hotspot | Impact |
|--------|---------|---------|--------|
| **Throughput** | 127.2 Mbps | 74.3 Mbps | **-41.5%** ⬇️ |
| **Latency** | 2.64 ms | 2.65 ms | Minimal |
| **Packet Drops** | 12 | **11,414** | **+95,016%** ⬆️ |

### Key Finding

**Hotspot traffic causes catastrophic packet loss** (11,414 vs 12 dropped packets), demonstrating the critical need for hybrid architecture with wireless failover capability.

## How to Run

### Compile
```bash
cd ~/ns-allinone-3.40/ns-3.40
./ns3 configure --enable-examples --enable-tests
./ns3 build
```

### Run Simulations
```bash
# Uniform traffic (baseline)
./ns3 run "hybrid-dcn-task4 --traffic=uniform"

# Hotspot traffic (80% to ToR 2)
./ns3 run "hybrid-dcn-task4 --traffic=hotspot"
```

### Save Results
```bash
./ns3 run "hybrid-dcn-task4 --traffic=uniform" > uniform_results.txt 2>&1
./ns3 run "hybrid-dcn-task4 --traffic=hotspot" > hotspot_results.txt 2>&1
```

## File Structure
```
04_Task4_Hybrid_Architecture/
├── README.md                          # This file
├── Code/
│   └── hybrid-dcn-task4.cc           # Simulation code
├── Results/
│   ├── task4_uniform_baseline.txt    # Uniform traffic results
│   └── task4_hotspot_baseline.txt    # Hotspot traffic results
├── Graphs/                            # (To be populated)
├── Scripts/                           # (To be populated)
└── Documentation/
    ├── Task4_Progress_Report.md      # Detailed progress report
    └── Relieving_hotspots_paper.pdf  # Reference: Shan et al. 2014
```

## Next Steps

### Phase 1: WiGig Infrastructure (Weeks 1-2)
- [ ] Add WiGig NetDevices to ToR 2 and ToR 3
- [ ] Configure 60 GHz wireless links between neighbors
- [ ] Set directional antennas (30-60° beamwidth)
- [ ] Model 4+ Gbps wireless capacity

### Phase 2: Congestion Detection (Week 3)
- [ ] Monitor queue occupancy at each ToR
- [ ] Implement cache occupancy rate (β) tracking
- [ ] Define failover threshold
- [ ] Add congestion detection logic

### Phase 3: Traffic Offloading (Week 4)
- [ ] Implement wireless failover mechanism
- [ ] Add load balancing across neighbors
- [ ] Route through under-utilized wired paths
- [ ] Verify no new hotspots created

### Phase 4: Analysis (Weeks 5-6)
- [ ] Run comprehensive experiments
- [ ] Compare hybrid vs wired-only
- [ ] Generate comparison graphs
- [ ] Document results for thesis

## Research Reference

This implementation is based on:

**"Relieving Hotspots in Data Center Networks with Wireless Neighborways"**  
Liqin Shan, Chang Zhao, Xiaohua Tian, Yu Cheng, Feng Yang, Xiaoying Gan  
*IEEE Globecom 2014*

Key concepts:
- Wireless neighborways for short-range offloading
- Cache occupancy rate (β) for hotspot detection
- Traffic offloading without creating new hotspots
- Leveraging under-utilized wired capacity

## Contact

Anthony Malone  
WiGig DCN Thesis Project  
2025
