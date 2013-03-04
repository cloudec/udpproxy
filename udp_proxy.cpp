#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <string>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include "udp_priv.h"
#include "udp_proxy.h"

using namespace std;
int UdpProxy::Init(ProxyConfig& config)
{
    uint32_t ip = 0;
    struct sockaddr_in sockaddr;
    struct ip_mreq mreq;
    int flag = 1;
    int ec;

    m_config = config;

    memset(&sockaddr, 0, sizeof(sockaddr));
    memset(&mreq, 0, sizeof(mreq));
    m_in_fd = socket(AF_INET, SOCK_DGRAM, 0);
    ec = m_in_fd;
    EC_CHECK_ERR_EXIT(ec);
    ip = Str2IP(m_config.m_in_ip);
    sockaddr.sin_family=AF_INET;
    if(IsMulticastIP(ip))
    {
        sockaddr.sin_addr.s_addr=htonl(0); 
    }
    else
    {
        sockaddr.sin_addr.s_addr=htonl(ip); 
    }
    sockaddr.sin_port=htons(m_config.m_in_port);
    ec = setsockopt(m_in_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    EC_CHECK_ERR_EXIT(ec);
    if(IsMulticastIP(ip))
    {
        inet_aton(m_config.m_in_ip.c_str(), &mreq.imr_multiaddr);
        ec = setsockopt(m_in_fd, SOL_IP, IP_ADD_MEMBERSHIP, &mreq,sizeof(mreq));
        EC_CHECK_ERR_EXIT(ec);
    }
    ec = SetSocketFlag(m_in_fd, O_NONBLOCK);
    EC_CHECK_ERR_EXIT(ec);
    if(ip != 0)
    {
        ec = bind(m_in_fd, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
        EC_CHECK_ERR_EXIT(ec);
    }
    else
    {
        ec = setsockopt(m_in_fd, SOL_SOCKET, SO_BINDTODEVICE,
                (char *)m_config.m_in_interface.c_str(),
                16);
        EC_CHECK_ERR_EXIT(ec);
    }

    memset(&sockaddr, 0, sizeof(sockaddr));
    memset(&mreq, 0, sizeof(mreq));
    m_out_fd = socket(AF_INET, SOCK_DGRAM, 0);
    EC_CHECK_ERR_EXIT(m_out_fd);
    if(IsMulticastIP(ip))
    {
        inet_aton(m_config.m_out_ip.c_str(), &mreq.imr_multiaddr);
        ec = setsockopt(m_out_fd, SOL_IP, IP_ADD_MEMBERSHIP, &mreq,sizeof(mreq));
        EC_CHECK_ERR_EXIT(ec);
    }
    ip = Str2IP(m_config.m_out_ip);
    sockaddr.sin_family=AF_INET;
    sockaddr.sin_addr.s_addr=htonl(ip); 
    sockaddr.sin_port=htons(m_config.m_out_port);
    ec = setsockopt(m_out_fd, SOL_SOCKET, SO_BINDTODEVICE,
            (char *)m_config.m_out_interface.c_str(),
            16);
    // ec = connect(m_out_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
    EC_CHECK_ERR_EXIT(ec);
    return 0;

err_exit:
    if(m_in_fd > 0)
    {
        close(m_in_fd);
        m_in_fd = -1;
    }
    if(m_out_fd > 0)
    {
        close(m_out_fd);
        m_out_fd = -1;
    }
    return ec;
}

int UdpProxy::Run()
{
    ssize_t length;
    UdpRawPacket packet;
    struct sockaddr_in sockaddr;
    uint32_t ip = Str2IP(m_config.m_out_ip);
    sockaddr.sin_family=AF_INET;
    sockaddr.sin_addr.s_addr=htonl(ip); 
    sockaddr.sin_port=htons(m_config.m_out_port);
    while(true)
    {
        packet.Reset();
        length = recvfrom(m_in_fd, (void *)packet.m_data, UDP_PACKET_LENGTH_MAX, 0, NULL, NULL);
        if(length < 0)
        {
            if(errno != EAGAIN)
            {
                cout << "Failed to recv data, errno: " << errno << "," << strerror(errno) << endl;
                break;
            }
            usleep(100 * 1000);
            continue;
        }
        m_counter.UpdateRecv(length);

        packet.m_length = length;
        while(packet.m_length > 0)
        {
            // length = send(m_out_fd, (void *)packet.m_data, packet.m_length, 0);
            length = sendto(m_out_fd, (void *)packet.m_data, packet.m_length, 0,
                    (struct sockaddr *)&sockaddr, sizeof(struct sockaddr));
            if(length < 0)
            {
                if(errno != EAGAIN)
                {
                    cout << "Failed to send data, errno: " << errno << "," << strerror(errno) << endl;
                    break;
                }
                usleep(1 * 1000);
                continue;
            }
            packet.m_length -= length;
            m_counter.UpdateSend(length);
        }
    }

    return 0;
}

int UdpProxyServer::Init(ServerConfig& config)
{
    int ec = -1;
    size_t index;

    m_config = config;
    UdpProxy* proxy = NULL;
    boost::thread *t;
    for(index = 0; index < m_config.m_proxies.size(); index ++)
    {
        cout << "new proxy " << endl;
        proxy = new UdpProxy();
        ec = proxy->Init(m_config.m_proxies[index]);
        EC_CHECK_ERR_EXIT(ec);
        t = new boost::thread(boost::bind(&UdpProxy::Run, proxy));
        m_threads.push_back(t);
        m_proxies.push_back(proxy);
    }

err_exit:
    if(ec < 0 && proxy != NULL)
    {
        delete proxy;
    }
    return ec;
}

int UdpProxyServer::Run()
{
    size_t index;
    ServerCounter counter;

    while(true)
    {
        cout << "server time: " << time(NULL) << endl;
        for(index = 0; index < m_proxies.size(); index ++)
        {
            m_proxies[index]->UpdateStatus(counter);
            cout << m_proxies[index]->GetName()
                << ": recv: " << counter.m_recv_bytes << " bytes(" << counter.m_recv_pkts << " pkts)"
                << ", send: " << counter.m_send_bytes << " bytes(" << counter.m_send_pkts << " pkts)"
                << ", time: " << counter.m_start << "-" << counter.m_end
                << endl;
        }
        sleep(10);
    }
    return 0;
}
