# 60 GHz WiGig–Augmented Data Center Networks  
## Simulation-Based MSc Thesis Research

**Student:** L00188373  
**Institution:** Atlantic Technological University (ATU)  
**Programme:** MSc in Data Analytics  
**Supervisor:** Dr. Saim Ghafoor  
**Status:** ✅ **Complete – Final Thesis Submission + Extended Challenges**

---

## 🎯 Research Objective
This research investigates the feasibility and performance impact of augmenting traditional wired data center networks (DCNs) with short-range 60 GHz WiGig (IEEE 802.11ad) wireless links. Using simulation-based analysis, the work evaluates whether high-throughput wireless *neighborways* can alleviate congestion, reduce packet loss, and improve latency for realistic data center traffic patterns dominated by mice and elephant flows.

---

## 🧠 Core Research Questions
- Can 60 GHz WiGig links operate reliably at typical data center rack distances?
- Do antenna beamwidth and modulation choices significantly affect performance?
- Can wireless neighborways mitigate hotspot congestion in wired DCNs?
- How do hybrid wired–wireless paths affect mice and elephant flows differently?
- What protocol-level challenges emerge when introducing wireless paths?

---

## 📊 Key Findings (Original Thesis Tasks)

### ✅ Task 1: Beamwidth Analysis
**Finding:** Antenna beamwidth (45°, 22.5°, 11.25°) has **no measurable impact on throughput** at typical rack distances (~2 m).  
**Result:** All configurations achieve ~2.64 Gbps.

**Implication:** Low-cost, 8-sector antennas are sufficient; expensive narrow-beam hardware is unnecessary for DCN deployments.

---

### ✅ Task 2: Modulation and Coding Scheme (MCS) Analysis
**Finding:** MCS 12 (16-QAM) provides the best balance between throughput and robustness.
- **MCS 12:** ~2.64 Gbps (≈ 2.6× faster than congested 1 Gbps uplinks)
- Higher MCS levels offer marginal gains but reduced reliability margin.

**Implication:** MCS 12 is optimal for short-range DCN wireless neighborways.

---

### ✅ Task 3: Distance Study
**Finding:** 60 GHz WiGig links remain stable across **1–10 m**, covering typical data center rack spacing.  
High MCS levels degrade only at longer distances.

**Implication:** WiGig is well-suited for rack-to-rack connectivity inside data centers.

---

### ✅ Task 4: Hybrid Wired–Wireless Architecture (Core Contribution)
A 4-rack, ToR–Spine DCN was simulated with a hotspot scenario where **80% of traffic targets a single rack**.

#### Wired-Only Baseline
- Severe congestion at the hotspot ToR
- High packet loss and increased latency
- Throughput collapse under hotspot load

#### Hybrid Architecture with 60 GHz Wireless Neighborways
Two 2.5 Gbps WiGig links were added between adjacent ToR switches, bypassing the congested spine layer.

**Results:**
- **Mice packet drops:** ↓ **42%**
- **Elephant packet drops:** ↓ **60%**
- **Average mice latency:** ↓ **30%**
- **Throughput improvement:**
  - Mice flows: **3.4×**
  - Elephant flows: **2.9×**

**Key Insight:**  
Wireless neighborways act as *express lanes* that offload hotspot traffic, improving both control-plane (mice) and data-plane (elephant) performance.

---

### ✅ Task 5: Blockage and Failover Analysis
**Finding:** WiGig exhibits a binary failure mode under blockage (~95% packet loss), but **automatic recovery occurs in <100 ms** once line-of-sight is restored.

**Implication:** Line-of-sight is critical; redundancy or backup paths are required for high availability.

---

## 🚀 Extended Challenges (December 2024 - January 2025)

Following supervisor requirements to address practical deployment challenges, each investigating three key questions: problem identification, NS-3 demonstration, and solution validation.

### ✅ Challenge 1: Blockage Sensitivity with Redundant Links
**Problem:** 60 GHz links fail completely when signal drops below -50 dBm (20-30 dB attenuation from obstruction)

**NS-3 Implementation:** 
- Error model with 100% packet loss during 10-15s blockage window
- Simulated realistic signal degradation using `blockage-severe.cc`
- Three scenarios tested: baseline, blockage without redundancy, blockage with redundancy

**Solution:** Redundant ToR1↔ToR3 link (2.5 Gbps) creating mesh topology

**Results:**
| Scenario | Throughput | Packet Loss | Improvement |
|----------|------------|-------------|-------------|
| Baseline (no blockage) | 13.2 Mbps | 0 | - |
| Blockage without redundancy | 9.9 Mbps | 198 packets | -25% |
| Blockage with redundancy | 25.3 Mbps | 0 | **+156%** |

**Key Finding:** Redundancy provides **156% throughput improvement** through multi-path capacity aggregation, not just failover.

[📊 View Graph](05_Extended_Challenges/Challenge1_Blockage/Challenge1_Final_Results.png) | [💻 View Code](05_Extended_Challenges/Challenge1_Blockage/blockage-severe.cc)

---

### ✅ Challenge 2: Beamforming and Alignment
**Status:** Complete - Results demonstrate beamwidth independence at 2m distance (see Task 1)

### 🔄 Challenge 3: Interference Management
**Status:** In Progress

### 📅 Challenge 4: Energy Efficiency
**Status:** Planned

### 📅 Challenge 5: Dynamic MCS Adaptation
**Status:** Planned

---

## ⚠️ Protocol-Level Insight: TCP Packet Reordering
During hybrid operation, some packet loss persists even when congestion is reduced. This is caused by **TCP packet reordering** due to differing wired and wireless path delays.

**Implication:**  
Reordering-tolerant mechanisms (e.g., TCP SACK, DCTCP with reordering control, or flow pinning) could further enhance hybrid performance.

---

## 🛠️ Technology Stack
- **Simulator:** ns-3.40
- **Wireless protocol:** IEEE 802.11ad (WiGig)
- **Frequency:** 60 GHz mmWave
- **Languages:** C++ (simulation), Python (analysis)
- **Visualization:** Matplotlib, Pandas

---

## 📁 Repository Structure
```
├── 00_Documentation/           # Methodology and presentations
├── 01_Simulation_Code/         # Base NS-3 simulations
├── 02_Task1_Beamwidth_Analysis/
├── 02_Task2_Complete_MCS_Analysis/
├── 03_Task3_Distance_Study/
├── 03_Task5_Blockage_Failover/
├── 04_Task4_Hybrid_Architecture/
├── 05_Extended_Challenges/     # NEW: Practical deployment challenges
│   └── Challenge1_Blockage/    # 156% improvement with redundancy
└── 05_Helper_Scripts/
```

---

## 🔮 Future Work
- **Multi-band hybrid design:** 60 GHz WiGig primary + 6 GHz Wi-Fi 6E backup
- **ML-based predictive failover** for blockage anticipation
- **Hardware testbed validation** of simulation results

---

## 📧 Contact
**Student:** L00188373  
**Repository:** https://github.com/L00188373/-wigig-dcn-thesis  
**Thesis Submission:** January 2025
