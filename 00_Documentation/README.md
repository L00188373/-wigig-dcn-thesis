# 60 GHz WiGig Data Center Network - Thesis Research

**Student:** L00188373  
**Institution:** ATU - Atlantic Technological University  
**Program:** MSc in Data Analytics  
**Last Updated:** November 17, 2025

## 📋 Repository Structure
```
├── 00_Documentation/           # Project documentation
├── 01_Simulation_Code/         # Main ns-3 simulation programs
├── 02_Task1_Beamwidth_Analysis/    # Beamwidth impact study
├── 03_Task5_Blockage_Failover/     # Link obstruction tests
├── 04_Partial_MCS_Analysis/        # MCS performance comparison
├── 05_Helper_Scripts/              # Utility scripts
└── 99_Archive/                     # Legacy files
```

## ✅ Completed Work (40%)

### Task 1: Beamwidth Analysis
**Finding:** At 1.82m, beamwidth (45°, 22.5°, 11.25°) doesn't affect throughput - all achieve 2,643 Mbps.

### Task 5: Blockage/Failover  
**Finding:** 95% packet loss during obstruction, automatic recovery in <100ms.

### Partial Task 2: MCS Analysis (3/24 complete)
- MCS 1: 249 Mbps
- MCS 12: 2,643 Mbps (88% efficiency - optimal)
- MCS 24: 3,665 Mbps

## 🎯 Remaining Work (60%)

- Complete MCS analysis (21 more tests)
- Distance variation study (0.5m to 10m)
- Hybrid architecture implementation

## 🔗 Links

- **GitHub:** https://github.com/L00188373/-wigig-dcn-thesis
- **Reference:** IEEE 802.11ad-2012 Standard
- **Validation:** Rohde & Schwarz WP 1MA220_0e

---

**For detailed methodology, see:** `METHODOLOGY_REPORT.md`
