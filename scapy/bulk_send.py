from scapy.all import *
import os

src_ip = "192.168.10.1"

dst_mac = "1c:57:dc:2e:a8:1d"   # macbook air
dst_ip = "192.168.10.114"

interface = "wlan0"

# 1. Create a packet
#packet_low_prio = Ether(dst=dst_mac) / IP(dst=dst_ip, tos=2) / TCP(dport=80, flags="S", seq=42)
#packet_mid_prio = Ether(dst=dst_mac) / IP(dst=dst_ip, tos=4) / TCP(dport=80, flags="S", seq=42)
#packet_high_prio = Ether(dst=dst_mac) / IP(dst=dst_ip, tos=6) / TCP(dport=80, flags="S", seq=42)



# 2. Send the packet
def bulk_send():
    # 构造IP数据包并设置TOS字段
    ip_pkt_low = Ether(dst=dst_mac) / IP(dst=dst_ip, tos=8<<2) / TCP(dport=80, flags="S", seq=100)
    ip_pkt_medium = Ether(dst=dst_mac) / IP(dst=dst_ip, tos=24<<2) / TCP(dport=80, flags="S", seq=200)
    ip_pkt_high = Ether(dst=dst_mac) / IP(dst=dst_ip, tos=48<<2) / TCP(dport=80, flags="S", seq=300)

    # load
    data = b"Hello, this is a large amount of data." * 10
    low_pkt = ip_pkt_low / Raw(load=data)
    medium_pkt = ip_pkt_medium /Raw(load=data)
    high_pkt = ip_pkt_high / Raw(load=data)

    i = 0
    while i < 100:
        sendp(low_pkt, iface=interface)
        sendp(medium_pkt, iface=interface)
        sendp(high_pkt, iface=interface)
        i += 1
        #time.sleep(0.01)

def tos_send():
    l2 = Ether(dst=dst_mac)/Dot1Q(vlan=10, prio=5)
    l3 = IP(dst=dst_ip, src=src_ip, tos=46<<2)
    l4 = UDP()

    for i in range(10):
        pkt = l2/l3/l4
        sendp(pkt, iface=interface)
        print("send out: ", pkt)

def main():
    #tos_send()
    bulk_send()

if __name__ == "__main__":
    print("Run as main")
    main()
