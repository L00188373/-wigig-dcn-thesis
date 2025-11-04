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
- **Use Case**: Supplementary links for elephant flow offloading and backup connectivity

## Technical Specifications
- **Simulator**: ns-3 version 3.40 with WiGig module
- **Standard**: IEEE 802.11ad (60 GHz)
- **Topology**: 4 racks, 2 active nodes at 1-2m spacing
- **Testing**: All 24 MCS levels, bidirectional traffic
- **Metrics**: Throughput, packet loss, latency
- **Propagation Model**: Friis free-space (idealized baseline)

## Quick Start

### Prerequisites
- ns-3.40 installed at `~/ns-allinone-3.40/ns-3.40`
- WiGig module in `contrib/` or `src/wigig/`

### Running the Simulation
```bash
cd ~/ns-allinone-3.40/ns-3.40
./ns3 run scratch/dcn-4node-modified
```

### Expected Runtime
Approximately 4-5 minutes (24 MCS levels × 10 seconds each)

## Repository Contents

### Source Code
- **`dcn-4node-modified.cc`** - Main simulation code (working version)
- **`dcn-4node-BACKUP.cc`** - Backup copy

### Results
- **`final_results.txt`** - Complete simulation output (all 24 MCS levels)
- **`mcs_data.csv`** - Parsed results in CSV format

### Analysis
- **`parse_results.py`** - Python script to extract data from results

## Key Findings

### Performance by MCS Level
| MCS | Modulation | Throughput | Packet Loss | Latency | Use Case |
|-----|------------|-----------|-------------|---------|----------|
| 1-4 | π/2-BPSK | 166-504 Mbps | 68-89% | 253-443ms | Emergency |
| 5-8 | π/2-QPSK | 546-988 Mbps | 65-78% | 152-239ms | Stable |
| **9-12** | **π/2-16QAM** | **1,051-1,877 Mbps** | **67-79%** | **85-143ms** | **Optimal** ⭐ |
| 17-24 | π/2-64QAM | 902-2,662 Mbps | 67-79% | 61-162ms | Maximum |

### Technology Comparison
| Technology | Throughput | Latency | Cabling | Flexibility |
|------------|-----------|---------|---------|-------------|
| Cat6 Ethernet | 1,000 Mbps | <1ms | Required | Low |
| **60 GHz WiGig** | **1,877 Mbps** | **85ms** | **None** | **High** |
| 10G Fiber | 10,000 Mbps | <0.5ms | Required | Low |

## Thesis Scope

### What is Modeled
✅ IEEE 802.11ad PHY layer (all 24 MCS levels)  
✅ Directional beamforming  
✅ MAC layer protocols (CSMA/CA, aggregation)  
✅ Bidirectional traffic flows  
✅ Free-space line-of-sight propagation  

### What is NOT Modeled
❌ Metallic multipath reflections  
❌ Multi-link interference  
❌ Building material effects  
❌ Dynamic blockage  
❌ Advanced beam nulling  

**Note**: Results represent optimistic baseline under idealized conditions. Real deployments would experience 20-40% performance degradation.

## Deployment Recommendations

### ✅ Suitable Use Cases
- Backup/failover connectivity between racks
- Temporary capacity expansion
- Flexible rack connections (avoid rewiring)
- Offloading elephant flows (bulk transfers, VM migration, backups)

### ❌ Not Recommended
- Primary high-throughput trunks
- Latency-sensitive applications (<10ms requirement)
- Dense multi-link scenarios without beam nulling
- Long-range connections (>5m)

## Project Status

- ✅ Simulation complete and validated
- ✅ All 24 MCS levels tested  
- ✅ Results documented and analyzed
- ✅ Comparison with industry prototypes (Qualcomm)
- 🔄 Thesis writing in progress
- 📅 Expected completion: [Your date]

## References

1. Qualcomm. "Wireless augmented data center." YouTube, Feb 2025. https://www.youtube.com/watch?v=VKy0zHhYaZY
2. IEEE 802.11ad-2012: "Enhancements for Very High Throughput in the 60 GHz Band"
3. ns-3 Network Simulator. https://www.nsnam.org/

## Supervision

**Supervisor**: [Supervisor Name]  
**Institution**: Atlantic Technological University  
**Programme**: MSc in Computing in Emerging Technologies  

## License

This code is for academic research purposes only.

---

*Last Updated: November 2025*
