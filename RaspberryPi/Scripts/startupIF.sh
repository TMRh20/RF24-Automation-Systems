ip tuntap add dev tun_nrf24 mode tun user pi multi_queue
ifconfig tun_nrf24 10.10.2.2/24

iptables -A OUTPUT -o tun_nrf24 -p tcp --sport 1883 -m state --state NEW,ESTABLISHED -j ACCEPT
iptables -A OUTPUT -p icmp -j ACCEPT
iptables -A OUTPUT -o tun_nrf24 -j DROP

iptables -A INPUT -i tun_nrf24 -p tcp --dport 1883 -m state --state NEW,ESTABLISHED -j ACCEPT
iptables -A INPUT -p icmp -j ACCEPT
iptables -A INPUT -i tun_nrf24 -j DROP

iptables -A FORWARD -i tun_nrf24 -o eth0 -j DROP
