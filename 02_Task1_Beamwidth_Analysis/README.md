# Task 1: Beamwidth Analysis

## Objective
Evaluate antenna beamwidth impact on WiGig performance at data center distances.

## Configuration
- MCS: 12 (16-QAM 13/16)
- Distance: 1.82 meters
- Sectors: 8, 16, 32
- Beamwidths: 45°, 22.5°, 11.25°

## Results

| Sectors | Beamwidth | Throughput | Loss |
|---------|-----------|------------|------|
| 8       | 45.00°    | 2,643.31 Mbps | 57.88% |
| 16      | 22.50°    | 2,643.31 Mbps | 57.88% |
| 32      | 11.25°    | 2,643.31 Mbps | 57.88% |

## Key Finding
**No significant throughput variation across beamwidths at short distances.**
Even wider beams provide sufficient gain at 1.82m.

## Files
- `Results/` - Raw simulation outputs
- `Graphs/` - PNG and PDF visualizations
- `Scripts/` - Python analysis code

## Implication
8-sector antennas sufficient for data center deployments, reducing cost.
