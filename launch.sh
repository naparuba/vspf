modprobe iptable_filter
modprobe ip_queue
iptables -A OUTPUT -p icmp -j QUEUE
iptables -A OUTPUT -p tcp -j QUEUE
iptables -A OUTPUT -p udp -j QUEUE
#le in fout la merde a debugger... foutu windows
#iptables -A INPUT -p icmp -j QUEUE
#iptables -A INPUT -p tcp -j QUEUE
#iptables -A INPUT -p udp -j QUEUE
./sin