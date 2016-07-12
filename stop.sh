iptables -F OUTPUT
iptables -L OUTPUT
iptables -F INPUT
iptables -L INPUT
modprobe -r ip_queue
modprobe -r iptable_filter