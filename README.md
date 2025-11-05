# 60 GHz WiGig Data Center Network Simulation

**Student**: Anthony Malone (L00188373) | **Institution**: ATU  
**Programme**: MSc Computing in Emerging Technologies

## Project
Performance Analysis of 60 GHz Wireless Links as Supplementary Interconnects in Data Center Networks

---

## Simulations

### 1. `dcn-4node-modified.cc` ✅ BASELINE
- All 24 IEEE 802.11ad MCS levels tested
- **Result**: MCS 12 = 1,877 Mbps (1.88× Cat6)
- Packet loss: 70%, Latency: 85ms

### 2. `dcn-hybrid-routing-final.cc` ✅ HYBRID ROUTING
- Dual-path: Cat6 (1Gbps) + WiGig (60GHz)
- Traffic classification: Mice → wired, Elephants → wireless
- **Result**: 2,100 Mbps combined (2.1× Cat6 alone)
- Based on ns-3-users expert feedback

### 3. `dcn-hybrid-routing.cc` 📋 PLACEHOLDER
- Early concept (use `-final` version instead)

### 4. `dcn-4node-BACKUP.cc` 📦 BACKUP

---

## Quick Start
```bash
# Baseline (5 min)
./ns3 run scratch/dcn-4node-modified

# Hybrid (10 sec)
./ns3 run scratch/dcn-hybrid-routing-final
```

---

## Key Results

| Test | Throughput | Loss | Latency | vs Cat6 |
|------|-----------|------|---------|---------|
| Baseline (MCS 12) | 1,877 Mbps | 70% | 85ms | 1.88× |
| Hybrid Total | 2,100 Mbps | 0%* | ~0ms* | 2.1× |

*Wired path performance; wireless varies by MCS

---

## Files

- `final_results.txt` - Baseline data (all 24 MCS)
- `hybrid-results.csv` - Hybrid test data
- `parse_results.py` - Data extraction tool

---

## Status

✅ Both simulations complete and validated  
✅ Expert feedback integrated (ns-3-users)  
✅ Traffic classification working  
🔄 Thesis writing in progress

---

## Technical

- **ns-3**: 3.40 (WiGig module)
- **Standard**: IEEE 802.11ad (60 GHz)
- **Distance**: 1-2m
- **Limitation**: Idealized LOS, no metallic multipath

---

**Contact**: L00188373@atu.ie
