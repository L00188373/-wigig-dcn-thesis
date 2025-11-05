# 60 GHz WiGig Data Center Network Simulation

**MSc Computing in Emerging Technologies - Thesis Project**  
**Student**: Anthony Malone  
**Student ID**: L00188373  
**Institution**: Atlantic Technological University (ATU)  
**Supervisor**: [Supervisor Name]

## Project Title
Performance Analysis of 60 GHz Wireless Links as Supplementary Interconnects in Data Center Networks: An ns-3 Simulation Study

## Overview
This project simulates IEEE 802.11ad (WiGig) wireless links between data center server racks at 1-2 meter distances using the ns-3.40 network simulator with the WiGig module.

**Research Question**: Can 60 GHz wireless technology supplement wired data center interconnects for specific use cases like elephant flow offloading and backup connectivity?

## Key Results
- **Optimal Configuration**: MCS 12 achieves 1,877 Mbps aggregate throughput
- **Peak Performance**: MCS 24 achieves 2,662 Mbps aggregate throughput  
- **Comparison**: 1.88× faster than Cat6 Ethernet (1 Gbps)
- **Trade-offs**: 70% packet loss, 85ms latency (suitable for bulk transfers, not real-time)

## Technical Specifications
- **Simulator**: ns-3 version 3.40
- **Module**: WiGig (IEEE 802.11ad)
- **Frequency**: 60 GHz unlicensed band
- **Topology**: 4 racks, 2 active nodes at 1-2m spacing
- **Tests**: All 24 MCS levels, bidirectional traffic
- **Duration**: 10 seconds per MCS level
- **Propagation Model**: Friis free-space (idealized baseline)

## Quick Start

### Prerequisites
- ns-3.40 installed with WiGig module in `contrib/` or `src/wigig/`

### Running the Simulation
```bash
cd ~/ns-allinone-3.40/ns-3.40
./ns3 run scratch/dcn-4node-modified
```

### Expected Output
- Runtime: ~4-5 minutes (24 MCS × 10 sec each)
- Results: Throughput, packet loss, latency for each MCS level
- Output saved to console and `final_results.txt`

## Repository Contents

### Source Code
- **`dcn-4node-modified.cc`** - Main simulation (working version)
- **`dcn-4node-BACKUP.cc`** - Backup copy

### Results Data
- **`final_results.txt`** - Complete simulation output (all 24 MCS levels)
- **`complete_results.txt`** - Subset of results
- **`mcs_data.csv`** - Parsed results in CSV format (optional)

### Analysis Tools
- **`parse_results.py`** - Python script to extract and format data

## Key Findings

### Optimal MCS Level: MCS 12 (π/2-16QAM)
- **Throughput**: 1,877 Mbps aggregate (bidirectional)
- **Packet Loss**: 70% (managed by TCP retransmissions)
- **Latency**: 85ms (acceptable for bulk transfers)
- **Use Case**: Best balance for 1-2m distances

### Use Case Recommendations
✅ **Suitable for**:
- Backup/failover links
- Elephant flow offloading (VM migration, backups)
- Temporary rack connections
- Flexible deployments

❌ **Not suitable for**:
- Latency-sensitive traffic (<10ms requirement)
- Primary high-throughput trunks
- Real-time applications

## Simulation Scope

### What is Modeled
✅ IEEE 802.11ad PHY (all 24 MCS levels)  
✅ Directional beamforming  
✅ MAC protocols (CSMA/CA, aggregation)  
✅ Bidirectional traffic flows  
✅ Free-space line-of-sight propagation  

### Limitations (What is NOT Modeled)
❌ Metallic multipath reflections  
❌ Multi-link interference  
❌ Building material effects  
❌ Dynamic blockage  
❌ Advanced beam nulling (Qualcomm technique)  

**Note**: Results represent optimistic baseline. Real deployments would experience 20-40% performance degradation due to environmental factors.

## Project Status
- ✅ Simulation complete and validated
- ✅ All 24 MCS levels tested  
- ✅ Results documented and analyzed
- 🔄 Thesis writing in progress
- 📅 Expected completion: [Month Year]

## References
1. IEEE 802.11ad-2012: "Enhancements for Very High Throughput in the 60 GHz Band"
2. Qualcomm. "Wireless augmented data center." YouTube, Feb 2025.
3. ns-3 Network Simulator. https://www.nsnam.org/

## Contact
**Student**: Anthony Malone  
**Email**: L00188373@atu.ie  
**Supervisor**: [Name]  
**Institution**: Atlantic Technological University

---
*Last Updated: November 2025*
