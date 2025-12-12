# 60 GHz WiGig–Augmented Data Center Networks  
## Simulation-Based MSc Thesis Research

**Student:** L00188373  
**Institution:** Atlantic Technological University (ATU)  
**Programme:** MSc in Data Analytics  
**Supervisor:** Dr. Saim Ghafoor  
**Status:** ✅ **Complete – Final Thesis Submission**

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

## 📊 Key Findings

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

## ⚠️ Protocol-Level Insight: TCP Packet Reordering
During hybrid operation, some packet loss persists even when congestion is reduced. This is caused by **TCP packet reordering**:

- Wired path: shorter (3 hops)
- Wireless neighborway: longer (4 hops)

Packets arrive out of order, and standard TCP misinterprets this as congestion, triggering unnecessary retransmissions and window reduction.

**Implication:**  
Reordering-tolerant mechanisms (e.g., TCP SACK, DCTCP with reordering control, or flow pinning) could further enhance hybrid performance.

---

## 🏗️ Simulated Architecture Overview
- **Topology:** 4 racks, 80 servers, ToR–Spine DCN
- **Hotspot:** One rack receives 80% of traffic
- **Wireless augmentation:** Short-range 60 GHz neighborways between adjacent ToRs
- **Traffic model:** Realistic mice (95% of flows) and elephant (5% of flows) distribution

---

## 🛠️ Technology Stack
- **Simulator:** ns-3.40
- **Wireless protocol:** IEEE 802.11ad (WiGig)
- **Frequency:** 60 GHz mmWave
- **Languages:** C++ (simulation), Python (analysis)
- **Visualization:** Matplotlib, Pandas

---

## 📁 Repository Structure
├── 00_Documentation/ # Thesis-ready methodology and reports
├── 01_Simulation_Code/ # ns-3 simulation programs
├── 02_Task1_Beamwidth_Analysis/ # Beamwidth experiments
├── 02_Task2_Complete_MCS_Analysis # MCS 1–24 evaluation
├── 03_Task3_Distance_Study # Distance feasibility analysis
├── 04_Task4_Hybrid_Architecture # Hybrid DCN simulations & results
├── 03_Task5_Blockage_Failover # Blockage and recovery analysis
├── 05_Helper_Scripts # Automation utilities
└── 99_Archive # Deprecated / historical code

---

## 📈 Research Contributions
1. Demonstrated beamwidth independence for 60 GHz links in DCNs  
2. Identified optimal MCS selection for short-range neighborways  
3. Validated WiGig feasibility across realistic rack distances  
4. Quantified hotspot mitigation benefits for mice and elephant flows  
5. Identified TCP reordering as a key challenge in hybrid DCNs  

---

## 🔮 Future Work
A natural extension of this work is a **multi-band hybrid design**, where:
- **60 GHz WiGig** provides high-throughput primary paths
- **6 GHz Wi-Fi 6E** serves as a lower-rate but more resilient backup

This would directly address mmWave reliability concerns while preserving the performance gains demonstrated in this thesis.

---

## 📚 Key References
- Shan et al., *Relieving Hotspots in Data Center Networks with Wireless Neighborways*, IEEE Globecom  
- Sur et al., *WiFi-Assisted 60 GHz Wireless Networks*, IEEE  
- Hamza et al., *Wireless Communication in Data Centers: A Survey*, IEEE Communications Surveys  

---

## 📧 Contact
**Student:** L00188373  
**Repository:** https://github.com/L00188373/-wigig-dcn-thesis
