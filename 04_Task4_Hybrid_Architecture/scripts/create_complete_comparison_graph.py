#!/usr/bin/env python3
"""
Task 4: Complete 4-Scenario Comparison
Shows Uniform vs Hotspot, and Wireless OFF vs ON
"""

import matplotlib.pyplot as plt
import numpy as np

# Data from all 4 scenarios
scenarios = ['Uniform\nWireless OFF', 'Uniform\nWireless ON', 
             'Hotspot\nWireless OFF', 'Hotspot\nWireless ON']

throughput = [127.2, 115.8, 74.3, 68.7]  # Mbps
latency = [2.64, 2.91, 2.65, 2.94]  # ms
dropped_packets = [12, 760, 11414, 4183]  # count

# Create figure with 3 subplots
fig, axes = plt.subplots(1, 3, figsize=(18, 6))
fig.suptitle('Task 4: Complete Comparison - All Four Scenarios\n(Uniform vs Hotspot Traffic × Wireless OFF vs ON)', 
             fontsize=15, fontweight='bold')

# Color scheme: Blue for uniform, Red for hotspot
colors = ['#3498db', '#85c1e9',  # Blues for uniform (dark, light)
          '#e74c3c', '#27ae60']  # Red for hotspot-OFF, Green for hotspot-ON

# ============================================
# Plot 1: Throughput
# ============================================
bars1 = axes[0].bar(scenarios, throughput, color=colors, alpha=0.8, edgecolor='black', linewidth=1.5)
axes[0].set_ylabel('Average Throughput (Mbps)', fontsize=12, fontweight='bold')
axes[0].set_title('Throughput Comparison', fontsize=13, fontweight='bold')
axes[0].set_ylim(0, max(throughput) * 1.25)
axes[0].grid(axis='y', alpha=0.3, linestyle='--')
axes[0].tick_params(axis='x', rotation=0, labelsize=9)

# Add value labels
for bar, val in zip(bars1, throughput):
    height = bar.get_height()
    axes[0].text(bar.get_x() + bar.get_width()/2., height,
                f'{val:.1f}',
                ha='center', va='bottom', fontsize=10, fontweight='bold')

# ============================================
# Plot 2: Latency
# ============================================
bars2 = axes[1].bar(scenarios, latency, color=colors, alpha=0.8, edgecolor='black', linewidth=1.5)
axes[1].set_ylabel('Average Latency (ms)', fontsize=12, fontweight='bold')
axes[1].set_title('Latency Comparison', fontsize=13, fontweight='bold')
axes[1].set_ylim(0, max(latency) * 1.25)
axes[1].grid(axis='y', alpha=0.3, linestyle='--')
axes[1].tick_params(axis='x', rotation=0, labelsize=9)

# Add value labels
for bar, val in zip(bars2, latency):
    height = bar.get_height()
    axes[1].text(bar.get_x() + bar.get_width()/2., height,
                f'{val:.2f}',
                ha='center', va='bottom', fontsize=10, fontweight='bold')

# ============================================
# Plot 3: Dropped Packets (LOG SCALE!)
# ============================================
bars3 = axes[2].bar(scenarios, dropped_packets, color=colors, alpha=0.8, edgecolor='black', linewidth=1.5)
axes[2].set_ylabel('Dropped Packets (log scale)', fontsize=12, fontweight='bold')
axes[2].set_title('Packet Loss Comparison', fontsize=13, fontweight='bold')
axes[2].set_yscale('log')  # Log scale because values range from 12 to 11,414
axes[2].grid(axis='y', alpha=0.3, linestyle='--', which='both')
axes[2].tick_params(axis='x', rotation=0, labelsize=9)

# Add value labels
for bar, val in zip(bars3, dropped_packets):
    height = bar.get_height()
    axes[2].text(bar.get_x() + bar.get_width()/2., height * 1.5,
                f'{val:,}',
                ha='center', va='bottom', fontsize=9, fontweight='bold')

# Add annotations for key findings
axes[2].annotate('', xy=(3, dropped_packets[2]), xytext=(3, dropped_packets[3]),
                arrowprops=dict(arrowstyle='<->', color='green', lw=3))
axes[2].text(3.15, (dropped_packets[2] + dropped_packets[3])/2, 
            '63%\nreduction',
            fontsize=11, fontweight='bold', color='green',
            bbox=dict(boxstyle='round', facecolor='lightgreen', alpha=0.8))

# Add legend
legend_elements = [
    plt.Rectangle((0,0),1,1, fc='#3498db', alpha=0.8, edgecolor='black', label='Uniform - Wireless OFF'),
    plt.Rectangle((0,0),1,1, fc='#85c1e9', alpha=0.8, edgecolor='black', label='Uniform - Wireless ON'),
    plt.Rectangle((0,0),1,1, fc='#e74c3c', alpha=0.8, edgecolor='black', label='Hotspot - Wireless OFF'),
    plt.Rectangle((0,0),1,1, fc='#27ae60', alpha=0.8, edgecolor='black', label='Hotspot - Wireless ON')
]
fig.legend(handles=legend_elements, loc='lower center', ncol=4, 
          fontsize=10, frameon=True, bbox_to_anchor=(0.5, -0.05))

# Add key findings footer
fig.text(0.5, 0.02, 
         '🎯 Key Findings: (1) Wireless adds overhead in normal traffic (12→760 drops)  |  '\
         '(2) Wireless provides massive relief during hotspot (11,414→4,183 drops = 63% reduction)',
         ha='center', fontsize=11, style='italic', fontweight='bold',
         bbox=dict(boxstyle='round', facecolor='yellow', alpha=0.4))

plt.tight_layout(rect=[0, 0.08, 1, 0.96])

# Save
plt.savefig('../Graphs/complete_4scenario_comparison.png', dpi=300, bbox_inches='tight')
plt.savefig('../Graphs/complete_4scenario_comparison.pdf', bbox_inches='tight')
print("✅ Graph saved to ../Graphs/complete_4scenario_comparison.png and .pdf")

plt.show()
