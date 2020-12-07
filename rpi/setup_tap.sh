sudo brctl addbr br0
sudo ip addr flush dev enp0s3
sudo brctl addif br0 enp0s3
sudo tunctl -t tap0 -u `whoami`
sudo brctl addif br0 tap0
sudo dhclient -v br0
sudo ifconfig enp0s3 up
sudo ifconfig tap0 up
sudo ifconfig br0 up
sudo bash -c 'echo "1" > /proc/sys/net/ipv4/ip_forward'
sudo iptables -t nat -A POSTROUTING -o br0 -j MASQUERADE