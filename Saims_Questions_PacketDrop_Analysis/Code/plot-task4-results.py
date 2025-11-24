#!/usr/bin/env python3
"""
Task 4 Results Visualization
Compares Wired-Only vs Hybrid Architecture Performance
Author: Anthony Malone (L00188373)
Date: November 24, 2025
"""

import matplotlib.pyplot as plt
import numpy as np

# Data from simulations
architectures = ['Wired-Only\nBaseline', 'Hybrid\n(+WiGig)']

# Metrics
throughput = [127.157, 115.763]  # Mbps
latency = [2.6412, 2.90545]       # ms
dropped_packets = [12, 760]       # count
packet_loss_pct = [0.0004, 0.0233] # percentage

# Color scheme
colors = ['#2E86AB', '#A23B72']  # Blue for wired, Purple for hybrid

# ============================================================================
# FIGURE 1: Packet Drops Comparison
# ============================================================================
fig1, ax1 = plt.subplots(figsize=(10, 6))

bars = ax1.bar(architectures, dropped_packets, color=colors, alpha=0.8, 
               edgecolor='black', linewidth=1.5)

# Add value labels on bars
for i, (bar, value) in enumerate(zip(bars, dropped_packets)):
    height = bar.get_height()
    ax1.text(bar.get_x() + bar.get_width()/2., height,
             f'{int(value)} packets',
             ha='center', va='bottom', fontsize=12, fontweight='bold')
    
    # Add percentage change for hybrid
    if i == 1:
        pct_change = ((dropped_packets[1] - dropped_packets[0]) / dropped_packets[0]) * 100
        ax1.text(bar.get_x() + bar.get_width()/2., height * 0.5,
                 f'+{pct_change:.0f}%\nincrease',
                 ha='center', va='center', fontsize=11, 
                 bbox=dict(boxstyle='round', facecolor='white', alpha=0.8),
                 color='red', fontweight='bold')

ax1.set_ylabel('Packet Drops (count)', fontsize=13, fontweight='bold')
ax1.set_title('Task 4: Packet Drops - Wired vs Hybrid Architecture\n' + 
              'Uniform Traffic, No Congestion', 
              fontsize=14, fontweight='bold', pad=20)
ax1.grid(axis='y', alpha=0.3, linestyle='--')
ax1.set_ylim(0, max(dropped_packets) * 1.2)

# Add annotation box
textstr = 'Key Finding:\n63× more drops with hybrid!\nCause: TCP packet reordering'
props = dict(boxstyle='round', facecolor='wheat', alpha=0.8)
ax1.text(0.98, 0.97, textstr, transform=ax1.transAxes, fontsize=11,
         verticalalignment='top', horizontalalignment='right', bbox=props)

plt.tight_layout()
plt.savefig('task4-packet-drops.png', dpi=300, bbox_inches='tight')
print("✓ Saved: task4-packet-drops.png")

# ============================================================================
# FIGURE 2: Performance Metrics Comparison (3-panel)
# ============================================================================
fig2, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(15, 5))

# Panel 1: Throughput
bars1 = ax1.bar(architectures, throughput, color=colors, alpha=0.8, 
                edgecolor='black', linewidth=1.5)
for bar, value in zip(bars1, throughput):
    height = bar.get_height()
    ax1.text(bar.get_x() + bar.get_width()/2., height,
             f'{value:.1f}\nMbps',
             ha='center', va='bottom', fontsize=11, fontweight='bold')

pct_change_tput = ((throughput[1] - throughput[0]) / throughput[0]) * 100
ax1.text(0.5, max(throughput) * 0.5, f'{pct_change_tput:.1f}%\ndecrease',
         ha='center', fontsize=10, color='red', fontweight='bold',
         bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))

ax1.set_ylabel('Throughput (Mbps)', fontsize=12, fontweight='bold')
ax1.set_title('Aggregate Throughput', fontsize=13, fontweight='bold')
ax1.grid(axis='y', alpha=0.3, linestyle='--')
ax1.set_ylim(0, max(throughput) * 1.15)

# Panel 2: Latency
bars2 = ax2.bar(architectures, latency, color=colors, alpha=0.8,
                edgecolor='black', linewidth=1.5)
for bar, value in zip(bars2, latency):
    height = bar.get_height()
    ax2.text(bar.get_x() + bar.get_width()/2., height,
             f'{value:.2f}\nms',
             ha='center', va='bottom', fontsize=11, fontweight='bold')

pct_change_lat = ((latency[1] - latency[0]) / latency[0]) * 100
ax2.text(0.5, max(latency) * 0.5, f'+{pct_change_lat:.1f}%\nincrease',
         ha='center', fontsize=10, color='red', fontweight='bold',
         bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))

ax2.set_ylabel('Average Latency (ms)', fontsize=12, fontweight='bold')
ax2.set_title('Average Latency', fontsize=13, fontweight='bold')
ax2.grid(axis='y', alpha=0.3, linestyle='--')
ax2.set_ylim(0, max(latency) * 1.15)

# Panel 3: Packet Loss %
bars3 = ax3.bar(architectures, packet_loss_pct, color=colors, alpha=0.8,
                edgecolor='black', linewidth=1.5)
for bar, value in zip(bars3, packet_loss_pct):
    height = bar.get_height()
    ax3.text(bar.get_x() + bar.get_width()/2., height,
             f'{value:.4f}%',
             ha='center', va='bottom', fontsize=11, fontweight='bold')

ax3.set_ylabel('Packet Loss (%)', fontsize=12, fontweight='bold')
ax3.set_title('Packet Loss Rate', fontsize=13, fontweight='bold')
ax3.grid(axis='y', alpha=0.3, linestyle='--')
ax3.set_ylim(0, max(packet_loss_pct) * 1.15)

fig2.suptitle('Task 4: Performance Degradation with Multi-Path Routing\n' +
              'All metrics worsen when wireless links added (uniform load, no congestion)',
              fontsize=14, fontweight='bold', y=1.02)

plt.tight_layout()
plt.savefig('task4-performance-metrics.png', dpi=300, bbox_inches='tight')
print("✓ Saved: task4-performance-metrics.png")

# ============================================================================
# FIGURE 3: Path Comparison Diagram (Conceptual)
# ============================================================================
fig3, ax = plt.subplots(figsize=(12, 8))
ax.axis('off')

# Title
ax.text(0.5, 0.95, 'Multi-Path Routing Problem:\nWhy Wireless Causes More Drops',
        ha='center', va='top', fontsize=16, fontweight='bold',
        transform=ax.transAxes)

# Wired-only scenario
ax.text(0.05, 0.80, 'WIRED-ONLY (Single Path):', fontsize=13, fontweight='bold',
        transform=ax.transAxes, color='#2E86AB')

wired_text = """
Path: Server → ToR → Spine → ToR → Server
Hops: 3
Delay: Consistent (all packets same path)
Result: 12 drops (0.0004%)

TCP sees: Pkt 1, 2, 3, 4, 5 (IN ORDER ✓)
"""
ax.text(0.05, 0.55, wired_text, fontsize=11, transform=ax.transAxes,
        bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.3),
        family='monospace')

# Hybrid scenario
ax.text(0.05, 0.45, 'HYBRID (Multiple Paths):', fontsize=13, fontweight='bold',
        transform=ax.transAxes, color='#A23B72')

hybrid_text = """
Path A: Server → ToR → Spine → ToR → Server (3 hops, fast)
Path B: Server → ToR → Spine → ToR → ToR (wireless) → Server (4 hops, slow)
Path C: Server → ToR → Spine → ToR → ToR (wireless) → Server (4 hops, slow)

ECMP routing alternates between paths!

Result: 760 drops (0.023%) — 63× increase!

TCP sees: Pkt 1, 3, 5, 2, 4 (OUT OF ORDER ✗)
         ↑ Packets arrive in wrong sequence!

TCP reaction:
  1. Detects gaps → "Packet loss!"
  2. Sends duplicate ACKs
  3. Triggers fast retransmit
  4. Congestion window collapses
  5. Throughput drops 9%
"""
ax.text(0.05, 0.02, hybrid_text, fontsize=10, transform=ax.transAxes,
        bbox=dict(boxstyle='round', facecolor='lavender', alpha=0.3),
        family='monospace')

plt.tight_layout()
plt.savefig('task4-multipath-problem.png', dpi=300, bbox_inches='tight')
print("✓ Saved: task4-multipath-problem.png")

# ============================================================================
# FIGURE 4: Summary Table
# ============================================================================
fig4, ax = plt.subplots(figsize=(12, 6))
ax.axis('off')

# Title
ax.text(0.5, 0.95, 'Task 4 Results Summary: Wired-Only vs Hybrid',
        ha='center', va='top', fontsize=16, fontweight='bold',
        transform=ax.transAxes)

# Table data
table_data = [
    ['Metric', 'Wired-Only', 'Hybrid (+WiGig)', 'Change', 'Interpretation'],
    ['Packet Drops', '12', '760', '+6,233%', '❌ Much worse'],
    ['Throughput', '127.2 Mbps', '115.8 Mbps', '-9.0%', '❌ Degraded'],
    ['Latency', '2.64 ms', '2.91 ms', '+10.2%', '❌ Increased'],
    ['Packet Loss', '0.0004%', '0.023%', '+58×', '❌ Higher'],
]

# Create table
table = ax.table(cellText=table_data, cellLoc='center', loc='center',
                bbox=[0.05, 0.3, 0.9, 0.55])

table.auto_set_font_size(False)
table.set_fontsize(11)
table.scale(1, 2.5)

# Style header row
for i in range(5):
    table[(0, i)].set_facecolor('#4A90E2')
    table[(0, i)].set_text_props(weight='bold', color='white')

# Style data rows
for i in range(1, 5):
    for j in range(5):
        if j < 3:
            table[(i, j)].set_facecolor('#E8F4F8')
        elif j == 3:
            table[(i, j)].set_facecolor('#FFE5E5')
        else:
            table[(i, j)].set_facecolor('#FFF4E5')

# Key finding box
finding_text = """
KEY FINDING:
Adding wireless links WITHOUT congestion makes performance WORSE, not better!

Root Cause: Multi-path TCP packet reordering
- ECMP routing splits flows across wired + wireless paths
- Different path lengths cause out-of-order delivery
- TCP misinterprets as packet loss
- Triggers unnecessary retransmissions
- Congestion window collapses → Lower throughput

Literature Support: Sur et al. (2017) observed 5× throughput reduction
with dual 60GHz+WiFi interfaces due to 50% out-of-order packets.
"""

ax.text(0.5, 0.12, finding_text, ha='center', va='top', fontsize=10,
        transform=ax.transAxes,
        bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))

plt.tight_layout()
plt.savefig('task4-summary-table.png', dpi=300, bbox_inches='tight')
print("✓ Saved: task4-summary-table.png")

print("\n" + "="*60)
print("All graphs generated successfully!")
print("="*60)
print("\nFiles created:")
print("  1. task4-packet-drops.png")
print("  2. task4-performance-metrics.png")
print("  3. task4-multipath-problem.png")
print("  4. task4-summary-table.png")
print("\nReady for Friday meeting! 🎯")
