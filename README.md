# 60 GHz WiGig Data Center Network Simulation

**MSc Computing in Emerging Technologies - Thesis Project**  
**Student**: Anthony Malone | **ID**: L00188373  
**Institution**: Atlantic Technological University (ATU)

## Project Title
Performance Analysis of 60 GHz Wireless Links as Supplementary Interconnects in Data Center Networks: An ns-3 Simulation Study

---

## Overview
Simulates IEEE 802.11ad (WiGig) wireless links between data center racks at 1-2m using ns-3.40.

**Research Question**: Can 60 GHz wireless supplement wired interconnects for elephant flow offloading and backup connectivity?

---

## Simulation Files

### 1. `dcn-4node-modified.cc` ✅ **PRIMARY - WORKING**
- **Status**: Complete and validated
- **Purpose**: Baseline 60 GHz performance characterization
- **Tests**: All 24 IEEE 802.11ad MCS levels
- **Metrics**: Throughput, packet loss, latency
- **Use for**: Current thesis results

**Run:**
```bash
cd ~/ns-allinone-3.40/ns-3.40
./ns3 run scratch/dcn-4node-modified
```

### 2. `dcn-hybrid-routing.cc` 🔄 **WORK IN PROGRESS**
- **Status**: Architecture defined, implementation pending
- **Purpose**: Demonstrates intelligent hybrid wired/wireless routing
- **Concept**: Traffic classification (mice flows → wired, elephant flows → wireless)
- **Based on**: Expert feedback from ns-3-users mailing list (Nov 2025)
  - *"Who's going to decide what link to use? You'll need a shim layer."*
- **Use for**: Future work demonstration

### 3. `dcn-4node-BACKUP.cc`
- Backup copy of original simulation

---

## Key Results

**Optimal: MCS 12 (π/2-16QAM)**
- Throughput: 1,877 Mbps (1.88× Cat6 Ethernet)
- Packet Loss: 70%
- Latency: 85ms

| MCS | Modulation | Throughput | Loss | Latency | Use Case |
|-----|------------|-----------|------|---------|----------|
| 1-4 | BPSK | 166-504 Mbps | 68-89% | 253-443ms | Worst case |
| 5-8 | QPSK | 546-988 Mbps | 65-78% | 152-239ms | Stable |
| **9-12** | **16QAM** | **1,051-1,877 Mbps** | **67-79%** | **85-143ms** | **Optimal** ⭐ |
| 17-24 | 64QAM | 902-2,662 Mbps | 67-79% | 61-162ms | Maximum |

---

## Technical Specs
- **Simulator**: ns-3.40 (WiGig module compatibility)
- **Standard**: IEEE 802.11ad (60 GHz)
- **Topology**: 4 racks, 2 active at 1-2m spacing
- **Propagation**: Friis free-space (idealized baseline)
- **Duration**: 10s per MCS level

---

## Scope & Limitations

**Modeled:** ✅ PHY/MAC protocols, directional beamforming, bidirectional traffic  
**NOT Modeled:** ❌ Metallic multipath, multi-link interference, building materials, routing layer

**Note**: Results = optimistic baseline. Real deployments: 20-40% lower due to environmental factors.

---

## Use Cases

✅ **Suitable**: Backup links, VM migration, bulk transfers, temporary connections  
❌ **Not Suitable**: Latency-sensitive traffic (<10ms), real-time apps, primary trunks

---

## Expert Feedback Integration

**Source**: ns-3-users mailing list (Nov 2025), suggested by supervisor  
**Key Points**:
1. Dense deployment → RF interference ("microwave oven" concern)
   - **Response**: Scoped to supplementary links only
2. Need routing decision layer ("shim layer")
   - **Response**: Created `dcn-hybrid-routing.cc`
3. Consider metallic environments
   - **Response**: Documented as idealized assumption

---

## Project Status

✅ Baseline simulation complete (24 MCS levels)  
✅ Results validated vs. industry prototypes (Qualcomm)  
✅ Expert community engagement  
🔄 Thesis writing in progress  
📅 Hybrid routing architecture (future work)

---

## Quick Start
```bash
# Run simulation
cd ~/ns-allinone-3.40/ns-3.40
./ns3 run scratch/dcn-4node-modified

# Parse results
python3 parse_results.py
```

**Output**: ~5 min runtime, results in `final_results.txt`

---

## File Summary

| File | Status | Purpose |
|------|--------|---------|
| `dcn-4node-modified.cc` | ✅ Working | Current thesis data |
| `dcn-hybrid-routing.cc` | 🔄 WIP | Future work demo |
| `dcn-4node-BACKUP.cc` | 📦 Backup | Emergency fallback |

---

## References

1. IEEE 802.11ad-2012
2. Qualcomm. "Wireless augmented data center." YouTube, Feb 2025.
3. ns-3-users mailing list, Nov 2025

---

## Contact

**Email**: L00188373@atu.ie  
**Programme**: MSc Computing in Emerging Technologies  
**Supervisor**: [Name]

---

*Last Updated: November 2025*
