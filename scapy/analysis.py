from scapy.all import *

packets = rdpcap("log.pcap")

# 1. Extract the packets
low_prio_packets = [p for p in packets if p[IP].tos == 0x20]
mid_prio_packets = [p for p in packets if p[IP].tos == 0x28]
high_prio_packets = [p for p in packets if p[IP].tos == 0x10]

# 2. Print the number of packets
print("Low priority packets: {}".format(len(low_prio_packets)))
print("Mid priority packets: {}".format(len(mid_prio_packets)))
print("High priority packets: {}".format(len(high_prio_packets)))