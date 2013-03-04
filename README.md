udpproxy
========

Udp Proxy util for proxy udp unicast to multicast and vice versa.
1)  udpproxy: The proxy server, recv from in_* config and send to out_*
    ./udpproxy udpproxy.ini
        The ini config file support multi proxy instances.

    ./udpproxy in_interface in_ip in_port out_interface out_ip out_port
        in_interface: Recv udp packets interface, if you want use this, in_ip should be 0 or multicast address.
        in_ip: Recv udp packets ip.
        in_port: Recv udp packets port.
        out_interface: Send udp packets interface, if you want use this, out_ip should be 0 or multicast address.
        out_ip: Send udp packets to ip.
        out_port: Send udp packets to port.

2)  udpsend: test util.
    udpsend out_ip out_port
2)  udprecv: test util.
    udprecv in_ip in_port
