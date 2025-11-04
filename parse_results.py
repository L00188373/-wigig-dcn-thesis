#!/usr/bin/env python3
import re

print("MCS,Throughput_Mbps,Packet_Loss_%,Avg_Latency_ms")

with open('final_results.txt', 'r') as f:
    content = f.read()
    
# Find all MCS sections
mcs_pattern = r'Traffic from 10\.0\.0\.1.*?Throughput: ([\d.]+) Mbps.*?Packet Loss: ([\d.]+) %.*?Avg Delay: ([\d.]+) ms.*?Traffic from 10\.0\.0\.2.*?Throughput: ([\d.]+) Mbps.*?Packet Loss: ([\d.]+) %.*?Avg Delay: ([\d.]+) ms.*?MCS (\d+): ([\d.]+) Mbit/s'

matches = re.findall(mcs_pattern, content, re.DOTALL)

for match in matches:
    thru1, loss1, delay1, thru2, loss2, delay2, mcs, total_thru = match
    # Calculate averages
    avg_thru = (float(thru1) + float(thru2)) / 2
    avg_loss = (float(loss1) + float(loss2)) / 2
    avg_delay = (float(delay1) + float(delay2)) / 2
    
    print(f"{mcs},{avg_thru:.2f},{avg_loss:.2f},{avg_delay:.2f}")
