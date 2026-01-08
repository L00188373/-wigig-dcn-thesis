# Challenge 1: Blockage Sensitivity Analysis

## Overview
This challenge addresses 60 GHz link blockage vulnerability and validates redundancy as a solution, as requested by Dr. Saim Ghafoor (Dec 12, 2024).

## Results Summary
- **Baseline:** 13.2 Mbps (no blockage)
- **Blockage without redundancy:** 9.9 Mbps (25% loss, 198 packets dropped)
- **Blockage with redundancy:** 25.3 Mbps (156% improvement, 0 packet loss)

## Key Files
- `blockage-severe.cc` - NS-3 simulation with error model blockage
- `challenge1-blockage.cc` - Extended Task 4 with blockage events
- `Challenge1_Final_Results.png` - Performance comparison graph
- `Challenge1_Final_Results.pdf` - Publication-ready graph

## How to Run
```bash
./ns3 run "blockage-severe --blockage=false --redundancy=false"  # Baseline
./ns3 run "blockage-severe --blockage=true --redundancy=false"   # Problem
./ns3 run "blockage-severe --blockage=true --redundancy=true"    # Solution
```

## Key Findings
1. 60 GHz links fail completely when signal drops to -50 dBm
2. Error model with 100% packet loss accurately simulates blockage
3. Redundant links provide 156% throughput improvement
4. Mesh topology essential for production deployment

## Answers to Dr. Ghafoor's Questions
1. **Problem:** 60 GHz links experience 25% throughput loss during blockage
2. **NS-3 Demo:** Error model simulates signal blockage (10-15s window)
3. **Solution:** Redundant ToR1↔ToR3 link maintains service with 156% improvement
