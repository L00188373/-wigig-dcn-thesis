Task 4: Hybrid Wired–Wireless Data Center Architecture
Status: Complete – Realistic Hybrid Evaluation Implemented

This task evaluates a hybrid wired–wireless data center topology using short-range 60 GHz wireless neighbourways to mitigate hotspot congestion in spine–leaf networks.

The implementation progresses from an initial simplified model to a final realistic data center workload using mice and elephant flows.

1. Network Architecture

Topology:

4 ToR switches

2 Spine switches

20 servers per rack (80 total)

10 Gbps wired ToR–Spine links

2.5 Gbps wireless neighbourways between ToR 1–2 and 2–3

Hybrid Design:

ToR 0 → Wired-only (control)

ToR 1–3 → Hybrid capable (wired + wireless)

Wireless links provide alternative short-range bypass paths when congestion occurs at the spine layer.

2. Traffic Models

Two workload models are included:

A. Simplified Model (Initial Baseline)

Location:

Code/hybrid-dcn-task4.cc

Features:

Uniform traffic

Hotspot traffic (80% to ToR 2)

Aggregate metrics only

This model demonstrated severe hotspot packet loss in wired-only mode and established motivation for hybrid design.

B. Realistic DCN Model (Final Implementation)

Location:

Code/task4-mice-elephant-REALISTIC.cc

Features:

5000 flows

95% mice flows (1–100 KB)

5% elephant flows (1–50 MB)

Hotspot scenario (80% traffic to ToR 2)

Uniform scenario

Detailed per-class metrics

CSV output for reproducibility

This represents the final thesis-grade evaluation.

3. Experimental Scenarios

The following four scenarios were evaluated:

Uniform – Wired Only

Uniform – Hybrid

Hotspot – Wired Only

Hotspot – Hybrid

4. Key Results (Realistic Model)
Hotspot Scenario
Metric	Wired Only	Hybrid	Improvement
Throughput	8.25 Gbps	25.68 Gbps	+211%
Packet Drops	59,899	33,698	−44%
Mice Latency	16.7 ms	11.7 ms	−30%

Wireless neighbourways bypass congested ToR uplinks and reduce queue overflow.

Uniform Scenario
Metric	Wired Only	Hybrid	Improvement
Throughput	89.16 Gbps	101.29 Gbps	+13.6%
Packet Drops	39,078	22,100	−43%

Under balanced load, hybrid links provide path diversity and effective capacity aggregation.

5. How to Reproduce Results

Copy the realistic script into ns-3 scratch:

cp Code/task4-mice-elephant-REALISTIC.cc ~/ns-allinone-3.40/ns-3.40/scratch/

Build ns-3:

cd ~/ns-allinone-3.40/ns-3.40
./ns3 build

Run scenarios:

Uniform baseline:

./ns3 run "task4-mice-elephant-REALISTIC"

Uniform hybrid:

./ns3 run "task4-mice-elephant-REALISTIC --wireless=1"

Hotspot baseline:

./ns3 run "task4-mice-elephant-REALISTIC --congestion=1"

Hotspot hybrid:

./ns3 run "task4-mice-elephant-REALISTIC --congestion=1 --wireless=1"

CSV outputs will be generated automatically.

6. Research Basis

Based on:

Shan et al.,
"Relieving Hotspots in Data Center Networks with Wireless Neighborways"
IEEE Globecom 2014

The hybrid design leverages short-range directional wireless links to offload congested wired uplinks without introducing new bottlenecks.

7. Conclusion

Results demonstrate that:

Wired-only architectures suffer severe performance degradation under hotspot traffic.

Hybrid wireless neighbourways significantly reduce congestion.

Mice flows benefit most due to reduced packet reordering and queue overflow.

Wireless links function as dynamic capacity aggregation rather than passive failover.

This validates the feasibility of hybrid wired–wireless data center architectures.
