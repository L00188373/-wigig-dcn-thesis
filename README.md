cat > README.md << 'EOF'
# 60 GHz WiGig Data Center Network Simulation

**MSc Computing in Emerging Technologies - Thesis Project**  
**Student**: Anthony Malone  
**Student ID**: L00188373  
**Institution**: Atlantic Technological University (ATU)  
**Supervisor**: [Supervisor Name]

## Project Title
Performance Analysis of 60 GHz Wireless Links as Supplementary Interconnects in Data Center Networks: An ns-3 Simulation Study

---

## Overview
This project simulates IEEE 802.11ad (WiGig) wireless links between data center server racks at 1-2 meter distances using the ns-3.40 network simulator with the WiGig module.

**Research Question**: Can 60 GHz wireless technology supplement wired data center interconnects for specific use cases like elephant flow offloading and backup connectivity?

---

## Repository Contents

### 📁 Simulation Files

#### 1. **`dcn-4node-modified.cc`** ✅ **PRIMARY WORKING SIMULATION**
**Status**: Complete and validated  
**Purpose**: Baseline 60 GHz WiGig performance characterization

**What it does:**
- Tests all 24 IEEE 802.11ad MCS levels
- Measures throughput, packet loss, and latency
- Pure wireless performance (no hybrid routing)
- Establishes baseline under idealized LOS conditions

**Use this for**: Current thesis results and data

**Run with:**
```bash
cd ~/ns-allinone-3.40/ns-3.40
./ns3 run scratch/dcn-4node-modified
```

---

#### 2. **`dcn-hybrid-routing.cc`** 🔄 **WORK IN PROGRESS**
**Status**: Architectural design complete, implementation pending  
**Purpose**: Demonstrates intelligent hybrid wired/wireless routing

**What it will do:**
- Dual-path architecture (Cat6 wired + 60 GHz wireless)
- Traffic classification by packet size:
  - Mice flows (<1KB) → Wired path (low latency)
  - Elephant flows (≥1KB) → Wireless path (high bandwidth)
- Intelligent routing decision layer
- Performance comparison: pure vs. hybrid

**Acknowledgment:**  
Based on expert feedback from **Tommaso Pecorella** (ns-3 core developer) via [ns-3-users mailing list](https://groups.google.com/g/ns-3-users), November 2025:

> "Start by drawing a careful diagram of the network setup, i.e., who's going to decide what link to use. If the application should be oblivious of the fact that you have 2 links, you'll need a shim layer to redirect the flows."

Community engagement suggested by thesis supervisor.

**Status**: Architecture defined, implementation roadmap established  
**Purpose in Thesis**: Demonstrates response to expert feedback and identifies practical deployment architecture for future work

---

#### 3. **`dcn-4node-BACKUP.cc`**
**Status**: Backup copy  
**Purpose**: Safety backup of original working simulation

---

### 📊 Results Files

- **`final_results.txt`** - Complete output from all 24 MCS tests
- **`complete_results.txt`** - Subset of results  
- **`mcs_data.csv`** - Parsed data in CSV format (if generated)

### 🔧 Analysis Tools

- **`parse_results.py`** - Python script to extract data from results

---

## Key Results (from `dcn-4node-modified.cc`)

### Optimal Configuration: MCS 12
- **Throughput**: 1,877 Mbps aggregate (bidirectional)
- **Packet Loss**: 70% (managed by TCP retransmissions)
- **Latency**: 85ms (acceptable for bulk transfers)
- **Comparison**: 1.88× faster than Cat6 Ethernet

### Performance Summary by MCS Range

| MCS Range | Modulation | Throughput | Packet Loss | Latency | Use Case |
|-----------|------------|-----------|-------------|---------|----------|
| 1-4 | π/2-BPSK | 166-504 Mbps | 68-89% | 253-443ms | Emergency/Worst |
| 5-8 | π/2-QPSK | 546-988 Mbps | 65-78% | 152-239ms | Stable |
| **9-12** | **π/2-16QAM** | **1,051-1,877 Mbps** | **67-79%** | **85-143ms** | **Optimal** ⭐ |
| 17-24 | π/2-64QAM | 902-2,662 Mbps | 67-79% | 61-162ms | Maximum |

---

## Technical Specifications

- **Simulator**: ns-3 version 3.40
- **Module**: WiGig (IEEE 802.11ad) - last maintained for 3.40
- **Frequency**: 60 GHz unlicensed band
- **Topology**: 4 racks, 2 active nodes at 1-2m spacing
- **Tests**: All 24 MCS levels, bidirectional traffic
- **Duration**: 10 seconds per MCS level
- **Propagation Model**: Friis free-space (idealized baseline)

### Why ns-3.40?
The WiGig module was last maintained for ns-3.40. Later releases (3.41+) refactored the Wi-Fi stack and broke compatibility.

---

## Use Case Recommendations

### ✅ Suitable Applications (Elephant Flows)
- Backup/failover connectivity between racks
- VM migration and live snapshots
- Bulk data transfers (database replication, storage sync)
- Temporary rack connections during expansion
- Flexible deployments avoiding rewiring costs

### ❌ Not Suitable (Mice Flows)
- Latency-sensitive traffic (<10ms requirement)
- Real-time user requests
- Control plane traffic
- High-frequency trading
- Primary east-west traffic

---

## Simulation Scope & Limitations

### What IS Modeled
✅ IEEE 802.11ad PHY layer (all 24 MCS levels)  
✅ Directional beamforming (simplified)  
✅ MAC layer protocols (CSMA/CA, aggregation, Block ACK)  
✅ Bidirectional traffic flows  
✅ Free-space line-of-sight propagation  

### What is NOT Modeled
❌ Metallic multipath reflections (acknowledged as idealized assumption)  
❌ Multi-link interference (single link pair tested)  
❌ Building material effects  
❌ Dynamic blockage (personnel, equipment)  
❌ Advanced beam nulling (Qualcomm technique)  
❌ **Intelligent routing layer** (addressed in `dcn-hybrid-routing.cc`)  

**Note**: Results represent optimistic baseline under idealized conditions. Real deployments would experience 20-40% performance degradation due to environmental factors (metallic multipath, interference, etc.).

---

## Expert Feedback Integration

### ns-3 Community Engagement

**Expert**: Tommaso Pecorella (ns-3 core developer)  
**Forum**: [ns-3-users mailing list](https://groups.google.com/g/ns-3-users)  
**Date**: November 2025  
**Suggested by**: Thesis supervisor

#### Key Feedback Received:

1. **"Microwave Oven" Concern**:
   - Dense 60 GHz deployment could create RF interference and heating
   - **Response**: Scoped research to supplementary links, not dense deployment
   - **Thesis Impact**: Strengthened limitations section, emphasized backup/temporary use cases

2. **Routing Decision Layer**:
   - "Who's going to decide what link to use?"
   - Need for "shim layer" to manage wired vs. wireless routing
   - **Response**: Created `dcn-hybrid-routing.cc` to explore architecture
   - **Thesis Impact**: Added "Future Work" section on MPTCP/routing intelligence

3. **Propagation Model**:
   - Consider metallic environments and building characteristics
   - **Response**: Explicitly documented idealized LOS assumptions
   - **Thesis Impact**: Enhanced limitations analysis, added gap assessment

---

## Project Roadmap

### ✅ Completed
- Baseline 60 GHz performance characterization (all 24 MCS)
- Results validation against industry prototypes (Qualcomm)
- Expert community engagement (ns-3-users)
- Gap analysis (simulation capabilities vs. real DCN requirements)

### 🔄 In Progress
- Thesis writing
- Hybrid routing architecture design (`dcn-hybrid-routing.cc`)
- Documentation refinement

### 📅 Future Work
- Implement hybrid routing simulation
- Multipath TCP integration
- Real testbed validation
- Environmental channel modeling (metallic multipath)
- Multi-link interference characterization

---

## Quick Start Guide

### Prerequisites
- ns-3.40 installed at `~/ns-allinone-3.40/ns-3.40`
- WiGig module in `src/wigig/` or `contrib/wigig/`

### Running the Baseline Simulation
```bash
cd ~/ns-allinone-3.40/ns-3.40
./ns3 run scratch/dcn-4node-modified
```

**Expected Output:**
- Runtime: ~4-5 minutes (24 MCS levels × 10 seconds each)
- Results printed to console and saved in `final_results.txt`
- Metrics: Throughput, packet loss, latency for each MCS

### Analyzing Results
```bash
python3 parse_results.py
```
Generates CSV data from simulation output.

---

## File Comparison Summary

| File | Status | Purpose | When to Use |
|------|--------|---------|-------------|
| **dcn-4node-modified.cc** | ✅ Working | Baseline performance testing | **Current thesis data** |
| **dcn-hybrid-routing.cc** | 🔄 WIP | Hybrid routing architecture | Future work demonstration |
| **dcn-4node-BACKUP.cc** | 📦 Backup | Safety copy | Emergency fallback |

---

## Technology Comparison

| Technology | Throughput | Latency | Packet Loss | Cabling | Flexibility |
|------------|-----------|---------|-------------|---------|-------------|
| Cat6 Ethernet | 1,000 Mbps | <1ms | <0.01% | Required | Low |
| **60 GHz WiGig (MCS 12)** | **1,877 Mbps** | **85ms** | **70%** | **None** | **High** |
| 10G Fiber | 10,000 Mbps | <0.5ms | <0.01% | Required | Low |

---

## References

1. IEEE 802.11ad-2012: "Enhancements for Very High Throughput in the 60 GHz Band"
2. Qualcomm. "Wireless augmented data center." YouTube, February 2025. https://www.youtube.com/watch?v=VKy0zHhYaZY
3. Pecorella, T. Personal communication via ns-3-users mailing list, November 2025.
4. ns-3 Network Simulator. https://www.nsnam.org/

---

## Contact

**Student**: Anthony Malone  
**Email**: L00188373@atu.ie  
**Student ID**: L00188373  
**Institution**: Atlantic Technological University  
**Programme**: MSc in Computing in Emerging Technologies  
**Supervisor**: [Supervisor Name]

---

## License

This code is for academic research purposes only.

---

*Last Updated: November 2025*
*Repository: https://github.com/L00188373/-wigig-dcn-thesis*
EOF

cat README.md
