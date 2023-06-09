from scapy.all import *
import os
import trace
import subprocess

dst_mac = "1c:57:dc:2e:a8:1d"
dst_ip = "192.168.10.114"

# 1. Create a packet
#packet_low_prio = Ether(dst=dst_mac) / IP(dst=dst_ip, tos=2) / TCP(dport=80, flags="S", seq=42)
#packet_mid_prio = Ether(dst=dst_mac) / IP(dst=dst_ip, tos=4) / TCP(dport=80, flags="S", seq=42)
#packet_high_prio = Ether(dst=dst_mac) / IP(dst=dst_ip, tos=6) / TCP(dport=80, flags="S", seq=42)


# 构造IP数据包并设置TOS字段
ip_pkt_voice = Ether(dst=dst_mac) / IP(dst=dst_ip, tos=0x20) / TCP(dport=80, flags="S", seq=42)     # 设置为语音类别，低5位为0x20
ip_pkt_video = Ether(dst=dst_mac) / IP(dst=dst_ip, tos=0x28) / TCP(dport=80, flags="S", seq=42)    # 设置为视频类别，低5位为0x28
ip_pkt_file = Ether(dst=dst_mac) / IP(dst=dst_ip, tos=0x10) / TCP(dport=80, flags="S", seq=42)    # 设置为文件传输类别，低5位为0x10


# 2. Send the packet
def bulk_send():
    while True:
        for i in range(10):
            sendp(ip_pkt_voice, iface="wlan0")
        sendp(ip_pkt_video, iface="wlan0")
        sendp(ip_pkt_file, iface="wlan0")
        #time.sleep(0.01)

def main():
    #bulk_send()
    sendp(ip_pkt_voice, iface="wlan0")
