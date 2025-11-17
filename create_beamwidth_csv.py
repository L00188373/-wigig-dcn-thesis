#!/usr/bin/env python3
import re
import csv

configs = [
    (8, 45.0),
    (16, 22.5),
    (32, 11.25)
]

results = []

for sectors, beamwidth in configs:
    filename = f'results_sectors{sectors}.txt'
    
    with open(filename, 'r') as f:
        content = f.read()
        
        throughput_match = re.search(r'Throughput:\s+([\d.]+)\s+Mbps', content)
        throughput = float(throughput_match.group(1)) if throughput_match else 0
        
        loss_match = re.search(r'Loss:\s+([\d.]+)\s+%', content)
        loss = float(loss_match.group(1)) if loss_match else 0
        
        results.append([sectors, beamwidth, throughput, loss])
        print(f"Sectors {sectors:2d} ({beamwidth:6.2f}°): {throughput:.2f} Mbps, Loss: {loss:.2f}%")

with open('beamwidth_analysis.csv', 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(['Sectors', 'Beamwidth_Degrees', 'Throughput_Mbps', 'PacketLoss_%'])
    writer.writerows(results)

print("\n✅ Results saved to beamwidth_analysis.csv")
