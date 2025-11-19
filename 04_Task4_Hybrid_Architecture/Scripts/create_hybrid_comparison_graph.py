#!/usr/bin/env python3
"""
Task 4: Hybrid Architecture Comparison Graph
Compares Wired-Only vs Wired+Wireless performance
"""

import matplotlib.pyplot as plt
import numpy as np

# Data from simulation results
configurations = ['Wired Only\n(Baseline)', 'Wired + Wireless\n(Hybrid)']

# Metrics
throughput = [74.3, 68.7]  # Mbps
latency = [2.65, 2.94]  # ms
dropped_packets = [11414, 4183]  # count

# Create figure with 3 subplots
fig, axes = plt.subplots(1, 3, figsize=(15, 5))
fig.suptitle('Task 4: Hybrid Architecture Performance - Hotspot Traffic Pattern\n80% of traffic concentrated at ToR 2', 
             fontsize=14, fontweight='bold')

# Colors
color_wired = '#e74c3c'  # Red for wired-only
color_hybrid = '#27ae60'  # Green for hybrid

colors = [color_wired, color_hybrid]

# Plot 1: Throughput
bars1 = axes[0].bar(configurations, throughput, color=colors, alpha=0.7, edgecolor='black', linewidth=1.5)
axes[0].set_ylabel('Average Throughput (Mbps)', fontsize=11, fontweight='bold')
axes[0].set_title('Throughput Comparison', fontsize=12, fontweight='bold')
axes[0].set_ylim(0, max(throughput) * 1.2)
axes[0].grid(axis='y', alpha=0.3, linestyle='--')

for i, (bar, val) in enumerate(zip(bars1, throughput)):
    height = bar.get_height()
    axes[0].text(bar.get_x() + bar.get_width()/2., height,
                f'{val:.1f} Mbps',
                ha='center', va='bottom', fontsize=10, fontweight='bold')

change_pct = ((throughput[1] - throughput[0]) / throughput[0]) * 100
axes[0].text(0.5, max(throughput) * 1.15, f'Change: {change_pct:+.1f}%',
            ha='center', fontsize=9, style='italic',
            bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))

# Plot 2: Latency
bars2 = axes[1].bar(configurations, latency, color=colors, alpha=0.7, edgecolor='black', linewidth=1.5)
axes[1].set_ylabel('Average Latency (ms)', fontsize=11, fontweight='bold')
axes[1].set_title('Latency Comparison', fontsize=12, fontweight='bold')
axes[1].set_ylim(0, max(latency) * 1.2)
axes[1].grid(axis='y', alpha=0.3, linestyle='--')

for i, (bar, val) in enumerate(zip(bars2, latency)):
    height = bar.get_height()
    axes[1].text(bar.get_x() + bar.get_width()/2., height,
                f'{val:.2f} ms',
                ha='center', va='bottom', fontsize=10, fontweight='bold')

change_pct = ((latency[1] - latency[0]) / latency[0]) * 100
axes[1].text(0.5, max(latency) * 1.15, f'Change: {change_pct:+.1f}%',
            ha='center', fontsize=9, style='italic',
            bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))

# Plot 3: Dropped Packets
bars3 = axes[2].bar(configurations, dropped_packets, color=colors, alpha=0.7, edgecolor='black', linewidth=1.5)
axes[2].set_ylabel('Dropped Packets', fontsize=11, fontweight='bold')
axes[2].set_title('Packet Loss Comparison', fontsize=12, fontweight='bold')
axes[2].set_ylim(0, max(dropped_packets) * 1.2)
axes[2].grid(axis='y', alpha=0.3, linestyle='--')

for i, (bar, val) in enumerate(zip(bars3, dropped_packets)):
    height = bar.get_height()
    axes[2].text(bar.get_x() + bar.get_width()/2., height,
                f'{val:,}',
                ha='center', va='bottom', fontsize=10, fontweight='bold')

reduction = ((dropped_packets[0] - dropped_packets[1]) / dropped_packets[0]) * 100
axes[2].text(0.5, max(dropped_packets) * 1.15, 
            f'🎯 REDUCTION: {reduction:.1f}%',
            ha='center', fontsize=10, fontweight='bold', color='green',
            bbox=dict(boxstyle='round', facecolor='lightgreen', alpha=0.8))

fig.text(0.5, 0.02, 
         'Key Finding: Wireless backup neighborways reduce packet drops by 63.4% with minimal latency impact',
         ha='center', fontsize=11, style='italic', 
         bbox=dict(boxstyle='round', facecolor='yellow', alpha=0.3))

plt.tight_layout(rect=[0, 0.05, 1, 0.96])

plt.savefig('../Graphs/hybrid_comparison.png', dpi=300, bbox_inches='tight')
plt.savefig('../Graphs/hybrid_comparison.pdf', bbox_inches='tight')
print("Graph saved to ../Graphs/hybrid_comparison.png and .pdf")

plt.show()
