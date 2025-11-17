# 60 GHz WiGig Data Center Network - Thesis Research

**Student:** L00188373  
**Institution:** ATU - Atlantic Technological University  
**Program:** MSc in Data Analytics  
**Supervisor:** Dr. Saim Ghafoor  
**Status:** In Progress (40% Complete)

---

## 🎯 Research Objective

Investigate the feasibility of using 60 GHz WiGig (IEEE 802.11ad) wireless technology as a high-throughput alternative to traditional wired infrastructure in data center environments.

---

## 📊 Key Findings (To Date)

### ✅ Task 1: Beamwidth Analysis (COMPLETE)
**Finding:** At typical data center distances (1.82m), antenna beamwidth (45°, 22.5°, 11.25°) does **not significantly impact throughput** - all configurations achieve 2,643 Mbps.

**Implication:** Cost-effective 8-sector antennas are sufficient; no need for expensive high-sector-count hardware.

### ✅ Task 5: Blockage with Automatic Failover (COMPLETE)
**Finding:** WiGig exhibits **binary failure mode** - 95% packet loss when obstructed, but automatic recovery in <100ms.

**Implication:** Line-of-sight is mandatory; redundant backup links essential for high availability.

### 🟡 Task 2: MCS Analysis (PARTIAL - 3/24)
**Finding:** MCS 12 achieves **88% efficiency** (2,643 Mbps from 3,000 theoretical), optimal for data centers.

- MCS 1 (BPSK): 249 Mbps (most robust)
- MCS 12 (16-QAM): 2,643 Mbps (optimal balance)
- MCS 24 (64-QAM): 3,665 Mbps (maximum speed)

---

## 📁 Repository Structure
```
├── 00_Documentation/              # Complete methodology and reports
│   ├── README.md                  # Detailed project overview
│   └── METHODOLOGY_REPORT.md      # Technical methodology (thesis-ready)
│
├── 01_Simulation_Code/            # ns-3.40 simulation programs
│   └── dcn-redundancy-failover.cc # Main configurable simulation
│
├── 02_Task1_Beamwidth_Analysis/   # ✅ COMPLETE
│   ├── Results/                   # Raw simulation outputs
│   ├── Graphs/                    # Publication-quality visualizations
│   └── Scripts/                   # Python analysis code
│
├── 03_Task5_Blockage_Failover/    # ✅ COMPLETE
│   ├── Results/                   # Test outputs
│   └── Graphs/                    # Failover timeline visualization
│
├── 04_Partial_MCS_Analysis/       # 🟡 IN PROGRESS (12.5% complete)
│   ├── Results/                   # MCS 1, 12, 24 test outputs
│   └── Graphs/                    # Comparison visualizations
│
├── 05_Helper_Scripts/             # Utility scripts
└── 99_Archive/                    # Legacy files
```

---

## 🔬 Technical Details

### Simulation Environment
- **Platform:** ns-3.40 with IEEE 802.11ad WiGig module
- **Frequency:** 60.48 GHz (Channel 2)
- **Bandwidth:** 2.16 GHz
- **Distance:** 1.82 meters (typical rack spacing)
- **Propagation:** Friis free-space loss model
- **Traffic:** UDP, 1472-byte packets, full line rate

### Configuration Options
```bash
./dcn-redundancy-failover --mcs=12 --distance=1.82 --sectors=8
```

Parameters:
- `--mcs` : Modulation scheme (1-24)
- `--distance` : Node separation (meters)
- `--sectors` : Antenna sectors (8, 16, 32)

---

## 📈 Progress Overview

| Task | Description | Status | Completion |
|------|-------------|--------|------------|
| **1** | **Beamwidth Analysis** | ✅ Complete | 100% |
| **2** | **MCS Performance Study** | 🟡 In Progress | 12.5% |
| **3** | **Distance Variation** | 📋 Planned | 0% |
| **4** | **Hybrid Architecture** | 📋 Planned | 0% |
| **5** | **Blockage & Failover** | ✅ Complete | 100% |

**Overall Progress:** 40% (2 of 5 scenarios complete)

---

## 🎓 Completed Deliverables

### Publications & Graphs
- ✅ 4-panel beamwidth analysis (PNG/PDF)
- ✅ Failover timeline visualization (PNG/PDF)
- ✅ MCS comparison graphs (PNG/PDF)
- ✅ Comprehensive methodology report (14 sections, thesis-ready)

### Code & Data
- ✅ Configurable ns-3 simulation (MCS, distance, sectors)
- ✅ Python analysis scripts (CSV extraction, graph generation)
- ✅ Raw simulation outputs (all tests preserved)
- ✅ Structured CSV data for further analysis

---

## 📚 Key References

- **IEEE 802.11ad-2012:** WiGig standard specification
- **Rohde & Schwarz WP 1MA220_0e:** "802.11ad – WLAN at 60 GHz" (validates our 88% efficiency)
- **ns-3 WiGig Module:** Implementation by Hany Assasa et al.

---

## 🚀 Next Steps

### Immediate (Week 2)
1. Complete MCS analysis (test MCS 2-11, 13-23)
2. Generate comprehensive MCS comparison graph

### Short-term (Weeks 3-4)
3. Distance variation study (0.5m to 10m)
4. Identify maximum viable distance for WiGig

### Medium-term (Weeks 5-6)
5. Hybrid architecture implementation (WiGig + Cat6 redundancy)
6. Traffic classification (mice vs elephant flows)

---

## 📖 Documentation

- **Quick Start:** See `00_Documentation/README.md`
- **Detailed Methodology:** See `00_Documentation/METHODOLOGY_REPORT.md`
- **Task-Specific Details:** Each task folder contains dedicated README

---

## 🎯 Research Questions

### Primary Question
*"Can 60 GHz WiGig technology provide a viable wireless alternative to wired infrastructure for data center rack-to-rack connections?"*

### Findings So Far
**YES, with constraints:**
- ✅ Achieves 2.6+ Gbps throughput (2.65× faster than Cat6)
- ✅ Sub-100ms failover capability
- ⚠️ Requires line-of-sight (95% loss when blocked)
- ⚠️ Limited to short distances (<5m expected)
- 🎯 **Recommendation:** Hybrid architecture (WiGig for elephant flows + wired backup)

---

## 📧 Contact

**Student:** L00188373  
**Institution:** ATU - Atlantic Technological University  
**Repository:** https://github.com/L00188373/-wigig-dcn-thesis

---

## 📊 Quick Stats

- **Lines of Code:** ~600 (main simulation)
- **Tests Run:** 8 complete scenarios
- **Data Points:** 3 MCS levels × 3 beamwidths + blockage tests
- **Graphs Generated:** 8 publication-quality visualizations
- **Documentation:** 17,000+ words (methodology report)

---

## 🏆 Key Achievement

**Validated IEEE 802.11ad specifications:** Our measured 88% efficiency (2,643/3,000 Mbps) matches industry white paper predictions (80-90% range), confirming simulation accuracy.

---

**Last Updated:** November 17, 2025  
**Repository Version:** 2.0 (Clean organized structure)
