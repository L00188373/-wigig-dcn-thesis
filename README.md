# On-Demand 60 GHz Wireless Neighbourways for Hotspot Relief in Data Centre Networks  
## Simulation-Based Evaluation using ns-3

**Student:** L00188373  
**Institution:** Atlantic Technological University (ATU)  
**Programme:** MSc in Computing in Cloud Technologies  
**Supervisor:** Dr. Saim Ghafoor  
**Status:** Thesis Submitted   

---

## 📘 Overview

Modern data center networks (DCNs) suffer from congestion hotspots caused by highly skewed traffic patterns and the coexistence of short-lived *mice* flows with large *elephant* transfers. While spine–leaf architectures provide redundancy, they cannot dynamically redistribute traffic without significant overprovisioning.

This project investigates whether **short-range 60 GHz WiGig (IEEE 802.11ad)** wireless links can **augment wired DCNs** using *neighborway* connections between adjacent Top-of-Rack (ToR) switches. Using **ns-3.40 simulations**, the work evaluates feasibility, performance gains, congestion mitigation, and protocol-level implications of **hybrid wired–wireless architectures**.

---

## 🎯 Research Objectives

This thesis addresses the following questions:

- Can 60 GHz WiGig links operate reliably at typical intra-rack and rack-to-rack distances?
- How do antenna beamwidth and modulation choices affect achievable throughput?
- Can wireless neighborways mitigate hotspot congestion in wired DCNs?
- How are mice and elephant flows affected in hybrid environments?
- What protocol-level challenges arise when introducing parallel wireless paths?

---

## 🧪 Methodology

All experiments were conducted using **ns-3.40** with IEEE 802.11ad (WiGig) models at 60 GHz.

**Simulated topology:**
- 4 ToR switches, 2 spine switches  
- 20 servers per rack (80 total)  
- 10 Gbps wired ToR–Spine links  
- 2.5 Gbps wireless neighborways between adjacent ToRs  

**Traffic workload:**
- 5000 flows  
- 95% mice (1–100 KB)  
- 5% elephant (1–50 MB)  

**Metrics:**
- Throughput  
- Packet loss  
- Latency  
- Congestion behavior  
- Evaluated under both **uniform** and **hotspot** traffic patterns  

---

## 📊 Summary of Thesis Tasks

### ✅ Task 1: Beamwidth Analysis
- Evaluated antenna sectors: **45°, 22.5°, 11.25°**
- **Result:** No measurable throughput difference (~2.64 Gbps) at 2 m
- **Conclusion:** Wide-beam, lower-cost antennas are sufficient for DCN neighborways

### ✅ Task 2: MCS Evaluation
- Tested multiple modulation and coding schemes
- **Finding:** **MCS 12 (16-QAM)** provides the best trade-off:
  - ~2.64 Gbps sustained throughput  
  - Stable short-range performance  
  - Better robustness than higher MCS levels  

### ✅ Task 3: Distance Study
- Evaluated link stability across **1–10 m**
- **Finding:** Stable performance across typical rack spacing; degradation only at extended distances

### ✅ Task 4: Hybrid Wired–Wireless Architecture (Core Contribution)

**Scenario:**  
- 4-rack DCN with **80% hotspot traffic** targeting one rack

**Wired-only baseline:**
- Severe ToR uplink congestion  
- High packet loss  
- Increased mice latency  
- Throughput collapse under hotspot load  

**Hybrid architecture:**
- Two **2.5 Gbps WiGig neighborways** added between adjacent ToRs  

**Hotspot Results:**

| Metric           | Wired Only | Hybrid   | Improvement |
|------------------|------------|----------|-------------|
| Total Throughput | 8.25 Gbps  | 25.68 Gbps | +211%       |
| Packet Drops     | 59,899     | 33,698   | −44%        |
| Mice Latency     | 16.7 ms    | 11.7 ms  | −30%        |

**Key Insight:**  
Wireless neighborways act as **dynamic congestion bypass paths**, improving both control-plane (mice) and bulk-transfer (elephant) performance.

### ✅ Task 5: Blockage and Failover Analysis
- Simulated line-of-sight obstruction of 60 GHz links
- **Findings:**
  - Near-total packet loss during blockage  
  - Automatic recovery in <100 ms after restoration  
- **Conclusion:** Redundancy or multipath support is required for availability

---

## 🚧 Extended Deployment Challenges

### Challenge: Blockage with Redundant Wireless Links

A redundant ToR-to-ToR link was added to form a **wireless mesh**:

| Scenario                     | Throughput | Packet Loss |
|-----------------------------|------------|-------------|
| Baseline                    | 13.2 Mbps  | 0           |
| Blockage (no redundancy)    | 9.9 Mbps   | 198 packets |
| Blockage (with redundancy)  | 25.3 Mbps  | 0           |

**Observation:**  
Redundancy enables both **failover** and **multi-path aggregation**, yielding performance gains beyond simple recovery.

---

## 🔬 Protocol-Level Observations

Hybrid operation introduces:

- TCP packet reordering due to path delay asymmetry  
- Residual packet drops unrelated to congestion  

This suggests the need for:
- Reordering-aware congestion control  
- Flow pinning  
- DCTCP or transport-layer adaptations  

---

## 🛠️ Technology Stack

- **Simulator:** ns-3.40  
- **Wireless Standard:** IEEE 802.11ad (WiGig)  
- **Frequency:** 60 GHz mmWave  
- **Languages:** C++, Python  
- **Analysis Tools:** Pandas, Matplotlib  

---

### 📁 Repository Structure

```text
├── 00_Documentation/
├── 01_Simulation_Code/
├── 02_Task1_Beamwidth_Analysis/
├── 02_Task2_Complete_MCS_Analysis/
├── 03_Task3_Distance_Study/
├── 03_Task5_Blockage_Failover/
├── 04_Task4_Hybrid_Architecture/
│   ├── Code/
│   ├── Results/
│   ├── Graphs/
│   └── Scripts/
├── 05_Extended_Challenges/
└── 05_Helper_Scripts/
---

## 🧠 Contributions

This work demonstrates that:

- 60 GHz WiGig is feasible for short-range DCN augmentation  
- Hybrid wired–wireless architectures significantly mitigate hotspot congestion  
- Mice flows benefit disproportionately from reduced queue overflow  
- Redundant wireless paths provide both resilience and extra capacity  
- Transport-layer considerations are critical for practical deployment  

---

## 🔮 Future Work

- Multi-band hybrid systems (60 GHz + sub-6 GHz fallback)  
- Adaptive MCS selection based on link conditions  
- ML-driven congestion prediction  
- Experimental hardware validation  

---

## 🔗 Repository

GitHub: https://github.com/L00188373/-wigig-dcn-thesis

---

## ℹ️ About

**MSc Thesis:** *On-Demand 60 GHz Wireless Neighbourways for Hotspot Relief in Data Centre Networks*
Simulation-based evaluation using ns-3.
