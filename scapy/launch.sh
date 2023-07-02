python3 bulk_send.py &
tcpdump -i wlan0 -w log.pcap &

echo "sleeping 60 seconds"
sleep 60

#kill -9 `ps -aux | grep bulk_send | awk '{print$2}'`
#kill -9 `ps -aux | grep tcpdump | awk '{print$2}'`

#python3 analysis.py

