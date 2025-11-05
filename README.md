# 60 GHz WiGig Data Center Network Simulation

**MSc Computing in Emerging Technologies - Thesis Project**  
**Student ID**: L00188373  
**Institution**: Atlantic Technological University (ATU)

## Project Title
Performance Analysis of 60 GHz Wireless Links as Supplementary Interconnects in Data Center Networks: An ns-3 Simulation Study

## Overview
This project simulates IEEE 802.11ad (WiGig) wireless links between data center server racks at 1-2 meter distances using the ns-3.40 network simulator.

## Key Results
- **Optimal Configuration**: MCS 12 achieves 1,877 Mbps aggregate throughput
- **Peak Performance**: MCS 24 achieves 2,662 Mbps aggregate throughput  
- **Comparison**: 1.88× faster than Cat6 Ethernet (1 Gbps)

## Quick Start
```bash
cd ~/ns-allinone-3.40/ns-3.40
./ns3 run scratch/dcn-4node-modified
```

## Repository Contents
- `dcn-4node-modified.cc` - Main simulation code
- `final_results.txt` - Complete results
- `mcs_data.csv` - Parsed data

## Status
✅ Simulation complete  
🔄 Thesis writing in progress
