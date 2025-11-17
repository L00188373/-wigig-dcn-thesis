# WiGig Simulation Programs

## Main Program: dcn-redundancy-failover.cc

**Configurable parameters:**
```bash
--mcs=X          # MCS level (1-24)
--distance=X.X   # Distance in meters
--sectors=X      # Antenna sectors (8, 16, 32)
--simulationTime=X.X  # Duration in seconds
```

**Usage:**
```bash
./dcn-redundancy-failover --mcs=12 --distance=1.82 --sectors=8
```

## Other Programs
- `dcn-hybrid-routing-final.cc` - Hybrid wired/wireless architecture
- `dcn-4node-modified.cc` - Multi-node baseline test

## Building
```bash
cd ~/ns-allinone-3.40/ns-3.40
./ns3 build
./build/scratch/ns3.40-dcn-redundancy-failover-default --mcs=12
```
