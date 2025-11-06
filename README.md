# 60 GHz WiGig Data Center Network Simulation

**Student**: Anthony Malone (L00188373)  
**Institution**: Atlantic Technological University (ATU)  
**Programme**: MSc Computing in Emerging Technologies

---

## Project Overview

This repository contains simulation code for investigating 60 GHz wireless links as supplementary interconnects in data center networks. The research examines whether augmenting traditional wired infrastructure (Cat6/fiber) with 60 GHz WiGig can alleviate network congestion through intelligent traffic classification.

**Important Note:** This repository contains exploratory simulation code developed during the research phase. The simulations demonstrate proof-of-concept for hybrid wired-wireless architectures using ns-3.40. Results presented are preliminary and reflect idealized conditions (free-space line-of-sight, no metallic multipath). Final validated results, comprehensive analysis, and deployment recommendations will be documented in the complete thesis submission. This work represents ongoing research into congestion relief mechanisms for oversubscribed data center networks.

---

## Simulation Files

### 1. `dcn-4node-modified.cc` - Baseline Characterization
**Purpose:** Pure 60 GHz wireless performance testing  
**Description:** Tests all 24 IEEE 802.11ad MCS levels to establish baseline wireless performance under idealized conditions

**Key Finding:** MCS 12 achieves 1,877 Mbps (1.88× Cat6 performance)

**Run:**
```bash
cd ~/ns-allinone-3.40/ns-3.40
./ns3 run scratch/dcn-4node-modified
```
**Runtime:** ~5 minutes

---

### 2. `dcn-hybrid-routing-final.cc` - Hybrid Architecture
**Purpose:** Dual-path congestion relief demonstration  
**Description:** Implements intelligent traffic classification with simultaneous wired (Cat6 1Gbps) and wireless (60 GHz WiGig) paths. Routes latency-sensitive traffic via wired and bandwidth-intensive traffic via wireless.

**Key Finding:** 2,100 Mbps combined throughput (2.1× Cat6 alone), demonstrates congestion relief

**Run:**
```bash
./ns3 run scratch/dcn-hybrid-routing-final
```
**Runtime:** ~10 seconds

---

### 3. `dcn-hybrid-routing.cc` - Early Concept
**Status:** Placeholder/documentation  
**Note:** Non-functional early design. Use `dcn-hybrid-routing-final.cc` for working implementation

---

## Research Questions

1. Can 60 GHz wireless provide sufficient capacity as supplementary data center interconnects?
2. Does adding wireless paths reduce congestion on oversubscribed wired links?
3. How should traffic be classified for optimal dual-path routing?
4. What performance trade-offs exist between pure wireless and hybrid architectures?

---

## Preliminary Results Summary

| Configuration | Throughput | Packet Loss | Use Case |
|---------------|-----------|-------------|----------|
| Cat6 Ethernet | 1,000 Mbps | <0.01% | Traditional wired |
| 60 GHz (MCS 12) | 1,877 Mbps | 70% | Wireless baseline |
| **Hybrid (Both)** | **2,100 Mbps** | **0%** | **Congestion relief** |

**Key Observation:** Hybrid architecture achieves 2.1× improvement with zero packet loss by intelligently routing traffic types to appropriate paths.

---

## Technical Specifications

- **Simulator:** ns-3 version 3.40
- **WiFi Module:** WiGig (IEEE 802.11ad)
- **Frequency:** 60 GHz unlicensed band
- **Distance:** 1-2 meters (rack-to-rack)
- **Propagation Model:** Friis free-space (idealized)
- **Topology:** 2-4 node configurations

**Limitations:** Simulations assume line-of-sight conditions without metallic multipath, building materials, or dense multi-link interference. Real deployments would experience additional performance degradation.

---

## Project Status

**Phase:** Code development and preliminary testing  
**Status:** Exploratory simulations complete  
**Next Steps:** Comprehensive analysis, literature integration, thesis writing

---

## Contact

**Email:** L00188373@atu.ie  
**Programme:** MSc Computing in Emerging Technologies  
**Institution:** Atlantic Technological University

---

*Last Updated: November 2025*  
*Repository contains preliminary research code - not final thesis submission*
