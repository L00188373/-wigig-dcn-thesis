#!/usr/bin/env python3
"""
Task 4 Topology Diagram with Traffic Flows
Creates a colored network diagram showing the hybrid DCN architecture

Author: Anthony Malone (L00188373)
Supervisor: Dr. Saim Ghafoor
Date: November 2024
"""

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import FancyBboxPatch, FancyArrowPatch, Circle
import numpy as np

# Create figure
fig, ax = plt.subplots(figsize=(16, 12))
ax.set_xlim(0, 16)
ax.set_ylim(0, 12)
ax.axis('off')

# Colors
COLOR_WIRED_ONLY = '#ff6b6b'      # Red - Wired only
COLOR_HYBRID = '#51cf66'           # Green - Hybrid
COLOR_SPINE = '#4dabf7'            # Blue - Spine
COLOR_SERVER = '#ffd43b'           # Yellow - Servers
COLOR_WIRELESS = '#9775fa'         # Purple - Wireless links
COLOR_HOTSPOT = '#ff8787'          # Light red - Hotspot traffic

# ========================================
# TITLE
# ========================================
ax.text(8, 11.5, 'Task 4: Hybrid DCN Topology with Traffic Flows', 
        ha='center', fontsize=20, fontweight='bold')
ax.text(8, 11, 'Hotspot Scenario: 80% Traffic → ToR 2', 
        ha='center', fontsize=14, style='italic', color='#ff6b6b')

# ========================================
# SPINE LAYER
# ========================================
spine_y = 9
spine_positions = [(6, spine_y), (10, spine_y)]

for i, (x, y) in enumerate(spine_positions):
    # Spine switch box
    box = FancyBboxPatch((x-0.8, y-0.4), 1.6, 0.8,
                         boxstyle="round,pad=0.1", 
                         facecolor=COLOR_SPINE, 
                         edgecolor='black', 
                         linewidth=2)
    ax.add_patch(box)
    ax.text(x, y, f'Spine {i}', ha='center', va='center', 
            fontweight='bold', fontsize=11, color='white')

# ========================================
# TOR LAYER
# ========================================
tor_y = 6
tor_positions = [
    (2, tor_y, 'ToR 0\nWIRED ONLY', COLOR_WIRED_ONLY),
    (6, tor_y, 'ToR 1\nHYBRID', COLOR_HYBRID),
    (10, tor_y, 'ToR 2\nHYBRID\n(HOTSPOT)', '#ff8787'),  # Special color for hotspot
    (14, tor_y, 'ToR 3\nHYBRID', COLOR_HYBRID)
]

tor_x_positions = {}
for i, (x, y, label, color) in enumerate(tor_positions):
    tor_x_positions[i] = x
    
    # ToR switch box
    box = FancyBboxPatch((x-0.9, y-0.5), 1.8, 1.0,
                         boxstyle="round,pad=0.1", 
                         facecolor=color, 
                         edgecolor='black', 
                         linewidth=3 if i == 2 else 2)  # Thicker border for hotspot
    ax.add_patch(box)
    ax.text(x, y, label, ha='center', va='center', 
            fontweight='bold', fontsize=10, color='white')

# ========================================
# WIRED UPLINKS (ToR → Spine)
# ========================================
for i, (tor_x, _, _, _) in enumerate(tor_positions):
    for spine_x, spine_y_pos in spine_positions:
        # Draw uplink
        arrow = FancyArrowPatch((tor_x, tor_y + 0.5), (spine_x, spine_y_pos - 0.4),
                               arrowstyle='-', 
                               color='black', 
                               linewidth=2,
                               linestyle='-')
        ax.add_patch(arrow)

# Add uplink label
ax.text(8, 7.5, 'Wired Uplinks: 1 Gbps', ha='center', fontsize=9, 
        bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))

# ========================================
# WIRELESS NEIGHBORWAYS
# ========================================
# ToR 1 ↔ ToR 2
wireless_y = tor_y + 0.8
arrow1 = FancyArrowPatch((tor_x_positions[1] + 0.9, wireless_y), 
                        (tor_x_positions[2] - 0.9, wireless_y),
                        arrowstyle='<->', 
                        color=COLOR_WIRELESS, 
                        linewidth=4,
                        linestyle='-',
                        mutation_scale=20)
ax.add_patch(arrow1)
ax.text((tor_x_positions[1] + tor_x_positions[2])/2, wireless_y + 0.3, 
        '2.5 Gbps WiGig\n(2m range)', 
        ha='center', fontsize=9, fontweight='bold',
        bbox=dict(boxstyle='round', facecolor=COLOR_WIRELESS, alpha=0.8, edgecolor='black'),
        color='white')

# ToR 2 ↔ ToR 3
arrow2 = FancyArrowPatch((tor_x_positions[2] + 0.9, wireless_y), 
                        (tor_x_positions[3] - 0.9, wireless_y),
                        arrowstyle='<->', 
                        color=COLOR_WIRELESS, 
                        linewidth=4,
                        linestyle='-',
                        mutation_scale=20)
ax.add_patch(arrow2)
ax.text((tor_x_positions[2] + tor_x_positions[3])/2, wireless_y + 0.3, 
        '2.5 Gbps WiGig\n(2m range)', 
        ha='center', fontsize=9, fontweight='bold',
        bbox=dict(boxstyle='round', facecolor=COLOR_WIRELESS, alpha=0.8, edgecolor='black'),
        color='white')

# ========================================
# SERVER RACKS
# ========================================
server_y = 3.5
for i, (tor_x, _, _, _) in enumerate(tor_positions):
    # Server rack representation (multiple small boxes)
    for row in range(2):
        for col in range(5):
            server_x = tor_x - 0.5 + col * 0.25
            server_y_pos = server_y - row * 0.3
            
            rect = plt.Rectangle((server_x, server_y_pos), 0.2, 0.25,
                                facecolor=COLOR_SERVER, 
                                edgecolor='black', 
                                linewidth=0.5)
            ax.add_patch(rect)
    
    # Server count label
    ax.text(tor_x, server_y - 1, f'20 servers', ha='center', fontsize=9,
            bbox=dict(boxstyle='round', facecolor='lightyellow', alpha=0.8))
    
    # Server to ToR link
    arrow = FancyArrowPatch((tor_x, server_y + 0.5), (tor_x, tor_y - 0.5),
                           arrowstyle='-', 
                           color='gray', 
                           linewidth=1.5,
                           linestyle='-')
    ax.add_patch(arrow)

# ========================================
# HOTSPOT TRAFFIC FLOWS
# ========================================
# Show traffic arrows from Rack 0, 1, 3 → Rack 2
hotspot_y = 4.5

# From Rack 0 → Rack 2
arrow_hotspot1 = FancyArrowPatch((tor_x_positions[0] + 0.5, hotspot_y), 
                                (tor_x_positions[2] - 1, hotspot_y),
                                arrowstyle='->', 
                                color=COLOR_HOTSPOT, 
                                linewidth=3,
                                alpha=0.7,
                                linestyle='--',
                                mutation_scale=20)
ax.add_patch(arrow_hotspot1)

# From Rack 1 → Rack 2
arrow_hotspot2 = FancyArrowPatch((tor_x_positions[1] + 0.5, hotspot_y + 0.2), 
                                (tor_x_positions[2] - 1, hotspot_y + 0.2),
                                arrowstyle='->', 
                                color=COLOR_HOTSPOT, 
                                linewidth=3,
                                alpha=0.7,
                                linestyle='--',
                                mutation_scale=20)
ax.add_patch(arrow_hotspot2)

# From Rack 3 → Rack 2
arrow_hotspot3 = FancyArrowPatch((tor_x_positions[3] - 0.5, hotspot_y + 0.4), 
                                (tor_x_positions[2] + 1, hotspot_y + 0.4),
                                arrowstyle='->', 
                                color=COLOR_HOTSPOT, 
                                linewidth=3,
                                alpha=0.7,
                                linestyle='--',
                                mutation_scale=20)
ax.add_patch(arrow_hotspot3)

# Hotspot label
ax.text(8, 5.2, '80% of Traffic → ToR 2 (Hotspot)', 
        ha='center', fontsize=11, fontweight='bold',
        bbox=dict(boxstyle='round', facecolor=COLOR_HOTSPOT, alpha=0.9, edgecolor='black'),
        color='white')

# ========================================
# WIRELESS OFFLOAD PATHS
# ========================================
# Show that Rack 1 & 3 can use wireless
ax.text(tor_x_positions[1], 2, 'CAN OFFLOAD\nVIA WIRELESS', 
        ha='center', fontsize=8, fontweight='bold',
        bbox=dict(boxstyle='round', facecolor='lightgreen', alpha=0.8),
        color='darkgreen')

ax.text(tor_x_positions[3], 2, 'CAN OFFLOAD\nVIA WIRELESS', 
        ha='center', fontsize=8, fontweight='bold',
        bbox=dict(boxstyle='round', facecolor='lightgreen', alpha=0.8),
        color='darkgreen')

ax.text(tor_x_positions[0], 2, 'MUST USE\nWIRED PATH', 
        ha='center', fontsize=8, fontweight='bold',
        bbox=dict(boxstyle='round', facecolor='#ffcccb', alpha=0.8),
        color='darkred')

# ========================================
# LEGEND
# ========================================
legend_y = 1
legend_items = [
    (COLOR_WIRED_ONLY, 'Wired-Only ToR'),
    (COLOR_HYBRID, 'Hybrid ToR (Wired + WiGig)'),
    (COLOR_SPINE, 'Spine Switch'),
    (COLOR_WIRELESS, 'WiGig Wireless Link (2.5 Gbps)'),
    (COLOR_HOTSPOT, 'Hotspot Traffic Flow (80%)'),
]

for i, (color, label) in enumerate(legend_items):
    x = 1 + (i % 3) * 5
    y = legend_y - (i // 3) * 0.5
    
    rect = plt.Rectangle((x, y-0.15), 0.3, 0.3, facecolor=color, edgecolor='black')
    ax.add_patch(rect)
    ax.text(x + 0.5, y, label, va='center', fontsize=9)

# ========================================
# KEY STATS BOX
# ========================================
stats_text = """KEY STATISTICS:
• Total: 80 servers, 4 ToR, 2 Spine
• Wireless Coverage: 66% (Racks 1 & 3)
• Hotspot Load: 80% → ToR 2
• Bottleneck: 2 × 1 Gbps = 2 Gbps
• Wireless Capacity: 2 × 2.5 Gbps = 5 Gbps"""

ax.text(13, 1.2, stats_text, 
        fontsize=9, family='monospace',
        bbox=dict(boxstyle='round', facecolor='lightyellow', alpha=0.9, edgecolor='black'),
        verticalalignment='top')

# ========================================
# TRAFFIC FLOW EXPLANATION
# ========================================
flow_text = """TRAFFIC FLOW:
1. Normal: Server → ToR → Spine → ToR → Server
2. Hotspot: 60 servers → ToR 2 (20 servers)
3. Wireless: ToR 1/3 → Direct WiGig → ToR 2
   (Bypasses congested spine layer)"""

ax.text(1, 1.2, flow_text, 
        fontsize=9, family='monospace',
        bbox=dict(boxstyle='round', facecolor='lightcyan', alpha=0.9, edgecolor='black'),
        verticalalignment='top')

# Save figure
plt.tight_layout()
output_file = 'task4_topology_diagram.png'
plt.savefig(output_file, dpi=300, bbox_inches='tight', facecolor='white')
print(f"✓ Topology diagram saved to: {output_file}")

plt.close()

print("\n" + "="*70)
print("TOPOLOGY DIAGRAM CREATED!")
print("="*70)
print("\nThe diagram shows:")
print("  ✓ 4-ToR, 2-Spine architecture")
print("  ✓ ToR 0: Wired-only (red)")
print("  ✓ ToR 1, 2, 3: Hybrid with WiGig (green)")
print("  ✓ ToR 2: Hotspot location (light red)")
print("  ✓ Wireless neighborways: ToR 1↔2, ToR 2↔3 (purple)")
print("  ✓ Hotspot traffic flows (red dashed arrows)")
print("  ✓ Wireless offload capability indicated")
print("\nReady for thesis and Friday meeting with Dr. Ghafoor!")
