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

### ✅ Task 2: MCS Analysis (COMPLETE - 24/24)
**Finding:** MCS 12 achieves **57% efficiency** (2,643 Mbps from 3,000 theoretical), optimal for data centers.
- MCS 1 (BPSK): 249 Mbps (most robust)
- MCS 12 (16-QAM): 2,643 Mbps (optimal balance)
- MCS 24 (64-QAM): 3,665 Mbps (maximum speed)

### ✅ Task 3: Distance Study (COMPLETE)
**Finding:** Distance analysis across various configurations and sectors.

### 🔄 Task 4: Hybrid Architecture Analysis (IN PROGRESS - Baseline Complete)
**Current Finding:** Hotspot traffic causes **catastrophic packet loss** in wired-only networks:
- Uniform traffic: 12 dropped packets
- Hotspot traffic (80% to one ToR): **11,414 dropped packets** (+95,016% increase)
- Throughput degradation: **41.5% reduction** (127.2 → 74.3 Mbps)

**Next Steps:** Implement WiGig wireless neighborways to offload congested traffic and relieve hotspots (based on Shan et al. 2014).

### ✅ Task 5: Blockage with Automatic Failover (COMPLETE)
**Finding:** WiGig exhibits **binary failure mode** - 95% packet loss when obstructed, but automatic recovery in <100ms.

**Implication:** Line-of-sight is mandatory; redundant backup links essential for high availability.

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
│   └── Scripts/                   # Python analysis tools
│
├── 02_Task2_Complete_MCS_Analysis/ # ✅ COMPLETE
│   ├── Results/                   # MCS 1-24 performance data
│   ├── Graphs/                    # Comparative visualizations
│   └── Scripts/                   # Automated testing scripts
│
├── 03_Task3_Distance_Study/       # ✅ COMPLETE
│   ├── Results/                   # Distance analysis data
│   ├── Graphs/                    # Distance vs performance plots
│   └── Scripts/                   # Analysis automation
│
├── 04_Task4_Hybrid_Architecture/  # 🔄 IN PROGRESS (Baseline Complete)
│   ├── Code/                      # hybrid-dcn-task4.cc
│   ├── Results/                   # Baseline uniform & hotspot results
│   ├── Documentation/             # Progress report & research papers
│   └── README.md                  # Task 4 specific documentation
│
├── 03_Task5_Blockage_Failover/    # ✅ COMPLETE
│   ├── Results/                   # Blockage scenario outputs
│   ├── Graphs/                    # Failover timing analysis
│   └── Scripts/                   # Visualization generators
│
├── 05_Helper_Scripts/             # Automation utilities
│   └── run_all_scenarios.sh       # Batch execution scripts
│
└── 99_Archive/                    # Historical/deprecated code
```

---

## 🔬 Current Task: Task 4 - Hybrid Architecture

### Status
**Phase 1:** ✅ Framework and baseline implementation complete  
**Phase 2:** ⏳ WiGig failover mechanism (next step)  
**Phase 3:** ⏸️ Pending - Congestion detection  
**Phase 4:** ⏸️ Pending - Analysis and documentation

### What's Working
- Simulation framework for 4-ToR, 2-Spine DCN
- Uniform and hotspot traffic pattern generation
- Baseline metrics collection (throughput, latency, packet drops)

### Next Implementation
Implement wireless neighborways based on:
> **"Relieving Hotspots in Data Center Networks with Wireless Neighborways"**  
> Shan et al., IEEE Globecom 2014

Expected improvement: Reduce packet drops from 11,414 to <1,000 during hotspot conditions.

---

## 🛠️ Technology Stack

- **Simulator:** ns-3.40 (Network Simulator 3)
- **Protocol:** IEEE 802.11ad (WiGig/DMG)
- **Frequency:** 60 GHz millimeter wave
- **Language:** C++ (simulation), Python (analysis)
- **Visualization:** Matplotlib, Pandas

---

## 📈 Research Impact

### Proven Benefits
1. **Cost Reduction:** 8-sector antennas sufficient (vs expensive 32-sector)
2. **Performance:** 2.6+ Gbps achievable at datacenter distances
3. **Resilience:** Sub-100ms automatic failover proven feasible

### Current Investigation
4. **Hotspot Relief:** Can WiGig neighborways reduce packet loss during traffic concentration?

---

## 📚 Key References

1. Nithin Michael et al., "An 802.11ad based Approach for High Throughput and Low Latency in Data Centers"
2. Shan et al., "Relieving Hotspots in Data Center Networks with Wireless Neighborways", IEEE Globecom 2014
3. ns-3 WiGig Module Documentation

---

## 📝 Usage

### Quick Start
```bash
# Clone repository
git clone https://github.com/L00188373/-wigig-dcn-thesis.git

# Copy simulation code to ns-3
cp 01_Simulation_Code/*.cc ~/ns-allinone-3.40/ns-3.40/scratch/

# Run current task (Task 4)
cd ~/ns-allinone-3.40/ns-3.40
./ns3 run "hybrid-dcn-task4 --traffic=hotspot"
```

### Running Specific Tasks
```bash
# Task 1: Beamwidth Analysis
./ns3 run "dcn-redundancy-failover --sectors=8"

# Task 4: Hybrid Architecture (uniform)
./ns3 run "hybrid-dcn-task4 --traffic=uniform"

# Task 4: Hybrid Architecture (hotspot)
./ns3 run "hybrid-dcn-task4 --traffic=hotspot"

# Task 5: Blockage/Failover
./ns3 run "dcn-redundancy-failover --blockage=1"
```

---

## 📅 Timeline

- ✅ Oct 2024: Environment setup, Task 1 (Beamwidth)
- ✅ Nov 2024: Tasks 2 (MCS), 3 (Distance), 5 (Failover)
- 🔄 Nov 2024: Task 4 baseline complete, WiGig implementation next
- ⏳ Dec 2024: Task 4 complete, final analysis
- ⏳ Jan 2025: Thesis writing and submission

---

## 🤝 Contributing

This is thesis research - not accepting external contributions. However, feedback and suggestions are welcome via issues.

---

## 📧 Contact

**Student:** L00188373  
**Repository:** [github.com/L00188373/-wigig-dcn-thesis](https://github.com/L00188373/-wigig-dcn-thesis)

---

**Last Updated:** November 19, 2025
