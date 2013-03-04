#include <iostream>
#include "udp_priv.h"
#include "config.h"
#include "udp_proxy.h"

using namespace std;
int usage()
{
    cout << "usage:" << endl;
    cout << "udpproxy in_interface in_ip in_port out_interface out_ip out_port" << endl;
    cout << "in_interface: Recv udp packets interface, if you want use this,"
        "in_ip should be 0 or multicast address." << endl;
    cout << "in_ip: Recv udp packets ip." << endl;
    cout << "in_port: Recv udp packets port." << endl;
    cout << "out_interface: Send udp packets interface, if you want use this,"
        "out_ip should be 0 or multicast address." << endl;
    cout << "out_ip: Send udp packets to ip." << endl;
    cout << "out_port: Send udp packets to port." << endl;
}

int main(int argc, char **argv)
{
    int ec;
    ServerConfig config;
    UdpProxyServer server;

    if(argc == 2)
    {
        string config_file = argv[1];
        ec = config.Init(config_file);
    }
    else
    {
        ec = config.Init(argc, argv);
    }
    EC_CHECK_RETURN(ec);
    ec = server.Init(config);
    EC_CHECK_RETURN(ec);
    ec = server.Run();
    EC_CHECK_RETURN(ec);

    return 0;
}
