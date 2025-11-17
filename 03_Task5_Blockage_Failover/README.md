# Task 5: Simple Blockage with Automatic Failover

## Objective
Demonstrate WiGig sensitivity to obstructions and automatic recovery.

## Test Design
```
Timeline:
├─ 0-5s:   Normal operation
├─ 5-10s:  Link blocked (TX power -50 dBm)
└─ 10-15s: Link recovered
```

## Results
- **Normal throughput:** 2,643 Mbps
- **Obstruction loss:** 95%
- **Recovery time:** <100ms (automatic)
- **Overall loss:** 57.88% (averaged over 15s)

## Key Finding
**WiGig exhibits binary failure mode** - link either works fully or fails completely. No graceful degradation.

## Files
- `Graphs/` - Failover timeline visualization
- `create_failover_graphs.py` - Graph generation script

## Supervisor Feedback
> "The results are interesting... they can also go into your thesis."
