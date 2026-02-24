60 GHz WiGig–Augmented Data Center Networks
Simulation-Based Evaluation of Hybrid Wired–Wireless Architectures

Student: L00188373
Institution: Atlantic Technological University (ATU)
Programme: MSc in Data Analytics
Supervisor: Dr. Saim Ghafoor
Status: ✅ Complete – Thesis Submitted

Abstract

Modern data center networks (DCNs) experience frequent congestion hotspots due to highly skewed traffic patterns and the dominance of short-lived “mice” flows alongside large “elephant” transfers. Traditional wired spine–leaf architectures provide redundancy but cannot dynamically redistribute traffic without significant overprovisioning.

This research investigates whether short-range 60 GHz WiGig (IEEE 802.11ad) wireless links can augment wired DCNs through neighborway connections between adjacent Top-of-Rack (ToR) switches. Using ns-3 simulation, the study evaluates feasibility, performance gains, congestion mitigation, and protocol-level implications of hybrid wired–wireless architectures.

Research Objectives

This thesis addresses the following research questions:

Can 60 GHz WiGig links operate reliably at typical intra-rack and rack-to-rack distances?

How do antenna beamwidth and modulation choices affect achievable throughput?

Can wireless neighborways mitigate hotspot congestion in wired DCNs?

How are mice and elephant flows affected differently in hybrid environments?

What protocol-level challenges arise when introducing parallel wireless paths?

Methodology

All experiments were conducted using ns-3.40 with IEEE 802.11ad (WiGig) models at 60 GHz. Simulations model:

4-rack spine–leaf topology

20 servers per rack (80 total)

10 Gbps wired ToR–Spine links

2.5 Gbps wireless neighborways between adjacent ToRs

Realistic workload:

95% mice flows (1–100 KB)

5% elephant flows (1–50 MB)

Performance metrics include throughput, packet loss, latency, and congestion behaviour under both uniform and hotspot traffic conditions.

Summary of Thesis Tasks
✅ Task 1: Beamwidth Analysis

Three antenna sector configurations were evaluated (45°, 22.5°, 11.25°).

Result:
No measurable throughput difference (~2.64 Gbps) at 2 m rack spacing.

Conclusion:
Wide-beam, lower-cost antennas are sufficient for DCN neighborway deployment.

✅ Task 2: Modulation and Coding Scheme (MCS) Evaluation

MCS levels were evaluated for throughput and reliability.

Finding:
MCS 12 (16-QAM) provides optimal balance:

~2.64 Gbps sustained throughput

Stable performance at short range

Superior robustness compared to higher MCS levels

✅ Task 3: Distance Study

Link stability evaluated across 1–10 m.

Finding:
WiGig links remain stable within typical data center rack spacing.
Higher MCS levels degrade only at extended distances beyond standard layouts.

✅ Task 4: Hybrid Wired–Wireless Architecture (Core Contribution)

A 4-rack DCN topology was simulated with an 80% hotspot targeting a single rack.

Wired-Only Baseline

Severe ToR uplink congestion

High packet loss

Increased latency for mice flows

Throughput degradation under hotspot load

Hybrid Architecture with Wireless Neighborways

Two 2.5 Gbps WiGig links were added between adjacent ToRs.

Hotspot Scenario Results:

Metric	Wired Only	Hybrid	Improvement
Total Throughput	8.25 Gbps	25.68 Gbps	+211%
Total Packet Drops	59,899	33,698	−44%
Mice Latency	16.7 ms	11.7 ms	−30%

Key Insight:
Wireless neighborways function as dynamic capacity augmentation paths, bypassing congested uplinks and improving both control-plane (mice) and bulk-transfer (elephant) performance.

✅ Task 5: Blockage and Failover Analysis

60 GHz links were evaluated under simulated obstruction.

Finding:

Blockage causes near-total packet loss during obstruction.

Automatic recovery occurs within <100 ms after line-of-sight restoration.

Conclusion:
Hybrid architectures require redundancy or multi-path support to maintain availability.

Extended Deployment Challenges

To address real-world deployment constraints, additional challenges were evaluated.

Challenge 1: Blockage with Redundant Wireless Links

A redundant ToR-to-ToR link was introduced to create a wireless mesh.

Scenario	Throughput	Packet Loss
Baseline	13.2 Mbps	0
Blockage (no redundancy)	9.9 Mbps	198 packets
Blockage (with redundancy)	25.3 Mbps	0

Observation:
Redundancy enables both failover and multi-path aggregation, yielding significant performance gains beyond simple recovery.

Protocol-Level Observations

Introducing parallel wired and wireless paths results in:

TCP packet reordering due to path delay asymmetry

Residual packet drops unrelated to congestion

Potential need for:

Reordering-aware congestion control

Flow pinning

DCTCP enhancements

This highlights the importance of transport-layer adaptation in hybrid DCNs.

Technology Stack

Simulator: ns-3.40

Wireless Standard: IEEE 802.11ad (WiGig)

Frequency: 60 GHz mmWave

Languages: C++, Python

Analysis Tools: Pandas, Matplotlib

Repository Structure
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
│   └── scripts/
├── 05_Extended_Challenges/
└── 05_Helper_Scripts/
Contributions

This thesis demonstrates that:

60 GHz WiGig is technically feasible for short-range DCN augmentation.

Hybrid wired–wireless architectures significantly mitigate hotspot congestion.

Mice flows benefit disproportionately from reduced queue overflow.

Redundant wireless paths provide both resilience and additional capacity.

Transport-layer considerations are critical for practical deployment.

Future Work

Multi-band hybrid systems (60 GHz + sub-6 GHz fallback)

Adaptive MCS selection based on link conditions

ML-driven congestion prediction

Experimental hardware validation

Repository

GitHub:
https://github.com/L00188373/-wigig-dcn-thesis
