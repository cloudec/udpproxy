#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include "udp_priv.h"

void usage()
{
    cout << "usage:" << endl;
    cout << "udpsend ip port" << endl;
}

int main(int argc, char **argv)
{
    usage();
    if(argc != 3)
    {
        return -1;
    }
    string ip_str = argv[1];
    uint32_t ip;
    uint16_t port = atoi(argv[2]);
    int fd;
    int ec;
    struct sockaddr_in sockaddr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ec = fd;
    EC_CHECK_RETURN(ec);

    ip = Str2IP(ip_str);
    if(IsMulticastIP(ip))
    {
        struct ip_mreq mreq;
        memset(&mreq, 0, sizeof(mreq));
        inet_aton(ip_str.c_str(), &mreq.imr_multiaddr);
        ec = setsockopt(fd, SOL_IP, IP_ADD_MEMBERSHIP, &mreq,sizeof(mreq));
        EC_CHECK_RETURN(ec);
    }
    sockaddr.sin_family=AF_INET;
    sockaddr.sin_addr.s_addr=htonl(ip); 
    sockaddr.sin_port=htons(port);
    // ec = connect(fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
    EC_CHECK_RETURN(ec);

    char data[1536];
    int length, data_length = 1024;
    while(true)
    {
        length = sendto(fd, (void *)data, data_length, 0,
                (struct sockaddr *)&sockaddr, sizeof(struct sockaddr));
        if(length < 0)
        {
            if(errno != EAGAIN)
            {
                cout << "Failed to send data, errno: " << errno << "," << strerror(errno) << endl;
                break;
            }
        }
        cout << "send data length " << length << endl;
        usleep(10 * 1000);
    }

    return 0;
}
