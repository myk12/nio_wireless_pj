set terminal pdf
set output "output.pdf"
set size ratio 0.3
set title "Throughput over Time (MBps)"
set xlabel "Time (s)"
set ylabel "Throughput"

set grid

plot "log/server_port_7777.dat" using 1:2 with lines linewidth 2 title "low priority", \
	"log/server_port_7778.dat" using 1:2 with lines linewidth 2 title "middle priority", \
	"log/server_port_7779.dat" using 1:2 with lines linewidth 2 title "high priority"
