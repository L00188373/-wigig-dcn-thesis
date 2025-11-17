#!/usr/bin/env python3
import matplotlib.pyplot as plt
import pandas as pd

plt.style.use('seaborn-v0_8-paper')

df = pd.read_csv('beamwidth_analysis.csv')

fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(16, 12))

# Graph 1: Throughput vs Sectors
ax1.plot(df['Sectors'], df['Throughput_Mbps'], 'o-', linewidth=3, markersize=12, color='#2E86AB')
ax1.set_xlabel('Number of Antenna Sectors', fontweight='bold', fontsize=12)
ax1.set_ylabel('Throughput (Mbps)', fontweight='bold', fontsize=12)
ax1.set_title('Throughput vs Antenna Sectors (MCS 12, 1.82m)', fontsize=14, fontweight='bold')
ax1.grid(True, alpha=0.3)
ax1.set_ylim([2500, 2700])

for i, row in df.iterrows():
    ax1.annotate(f'{row["Throughput_Mbps"]:.2f} Mbps', 
                (row['Sectors'], row['Throughput_Mbps']),
                xytext=(0, 15), textcoords='offset points',
                ha='center', fontweight='bold', fontsize=10)

# Graph 2: Throughput vs Beamwidth
ax2.plot(df['Beamwidth_Degrees'], df['Throughput_Mbps'], 'o-', linewidth=3, markersize=12, color='#C73E1D')
ax2.set_xlabel('Beamwidth (Degrees)', fontweight='bold', fontsize=12)
ax2.set_ylabel('Throughput (Mbps)', fontweight='bold', fontsize=12)
ax2.set_title('Throughput vs Beamwidth (MCS 12, 1.82m)', fontsize=14, fontweight='bold')
ax2.grid(True, alpha=0.3)
ax2.invert_xaxis()
ax2.set_ylim([2500, 2700])

for i, row in df.iterrows():
    ax2.annotate(f'{row["Throughput_Mbps"]:.2f} Mbps', 
                (row['Beamwidth_Degrees'], row['Throughput_Mbps']),
                xytext=(0, 15), textcoords='offset points',
                ha='center', fontweight='bold', fontsize=10)

# Graph 3: Packet Loss vs Beamwidth
ax3.plot(df['Beamwidth_Degrees'], df['PacketLoss_%'], 'o-', linewidth=3, markersize=12, color='#F18F01')
ax3.set_xlabel('Beamwidth (Degrees)', fontweight='bold', fontsize=12)
ax3.set_ylabel('Packet Loss (%)', fontweight='bold', fontsize=12)
ax3.set_title('Packet Loss vs Beamwidth (includes obstruction period)', fontsize=14, fontweight='bold')
ax3.grid(True, alpha=0.3)
ax3.invert_xaxis()
ax3.set_ylim([50, 65])

for i, row in df.iterrows():
    ax3.annotate(f'{row["PacketLoss_%"]:.2f}%', 
                (row['Beamwidth_Degrees'], row['PacketLoss_%']),
                xytext=(0, 10), textcoords='offset points',
                ha='center', fontweight='bold', fontsize=10)

# Graph 4: Summary Table
ax4.axis('off')
table_data = []
table_data.append(['Sectors', 'Beamwidth', 'Throughput', 'Loss'])
for _, row in df.iterrows():
    table_data.append([
        f"{int(row['Sectors'])}",
        f"{row['Beamwidth_Degrees']:.2f}°",
        f"{row['Throughput_Mbps']:.2f} Mbps",
        f"{row['PacketLoss_%']:.2f}%"
    ])

table = ax4.table(cellText=table_data, cellLoc='center', loc='center',
                  colWidths=[0.15, 0.25, 0.3, 0.3])
table.auto_set_font_size(False)
table.set_fontsize(12)
table.scale(1, 3)

for i in range(len(table_data)):
    if i == 0:
        for j in range(4):
            table[(i, j)].set_facecolor('#2E86AB')
            table[(i, j)].set_text_props(weight='bold', color='white')
    else:
        for j in range(4):
            table[(i, j)].set_facecolor('#F0F0F0' if i % 2 == 0 else 'white')

ax4.set_title('Beamwidth Analysis Summary', fontsize=14, fontweight='bold', pad=20)

fig.suptitle('Task 1: Beamwidth Analysis - WiGig Performance',
            fontsize=16, fontweight='bold', y=0.995)

plt.tight_layout(rect=[0, 0, 1, 0.99])

plt.savefig('beamwidth_analysis.png', dpi=300, bbox_inches='tight')
plt.savefig('beamwidth_analysis.pdf', dpi=300, bbox_inches='tight')

print("\n✅ Graphs generated!")
print("📊 Files: beamwidth_analysis.png, beamwidth_analysis.pdf")
print("\n" + "="*70)
print("KEY FINDING:")
print("="*70)
print("At 1.82m distance, beamwidth does NOT significantly affect throughput.")
print("All configurations (8, 16, 32 sectors) achieve 2,643 Mbps.")
print("Conclusion: At short distances, even wider beams provide sufficient gain.")
print("="*70)

plt.show()
