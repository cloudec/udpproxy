#ifndef __UDP_PROXY_H__
#define __UDP_PROXY_H__
#include <queue>
#include <boost/thread/thread.hpp>
#include "string.h"
#include "config.h"

#define UDP_PACKET_LENGTH_MAX 4096
struct UdpRawPacket
{
    char m_data[UDP_PACKET_LENGTH_MAX];
    ssize_t m_length;

    int Init(ssize_t length, char *data)
    {
        m_length = length;
        if(m_length > UDP_PACKET_LENGTH_MAX || m_length < 0)
        {
            m_length= 0;
            return -1;
        }
        memcpy(m_data, data, m_length);
        return 0;
    }
    void Reset()
    {
        m_length = 0;
    }
};
class UdpPacket
{
    public:
        UdpRawPacket *m_raw_packet;

    public:
        UdpPacket() { }
        ~UdpPacket()
        {
            if(m_raw_packet != NULL)
            {
                delete m_raw_packet;
            }
        }
};

class ServerCounter
{
    public:
        long m_recv_bytes;
        long m_recv_pkts;
        long m_send_bytes;
        long m_send_pkts;
        time_t m_start;
        time_t m_end;
    public:
        ServerCounter()
        {
            m_recv_bytes = 0;
            m_recv_pkts = 0;
            m_send_bytes = 0;
            m_send_pkts = 0;
        }
        void UpdateRecv(int recv_bytes)
        {
            m_recv_pkts ++;
            m_recv_bytes += recv_bytes;
        }
        void UpdateSend(int send_bytes)
        {
            m_send_pkts ++;
            m_send_bytes += send_bytes;
        }
};

#define INVALID_FD (-1)
class UdpProxy
{
    private:
        ProxyConfig m_config;
        int m_in_fd;
        int m_out_fd;
        std::queue<UdpPacket> m_packet_q;
        ServerCounter m_counter;
        ServerCounter m_saved_counter;

    public:
        UdpProxy()
        {
            m_in_fd = INVALID_FD;
            m_out_fd = INVALID_FD;
        }

        ~UdpProxy()
        {
            if(m_in_fd > 0)
            {
                close(m_in_fd);
                m_in_fd = INVALID_FD;
            }
            if(m_out_fd > 0)
            {
                close(m_out_fd);
                m_out_fd = INVALID_FD;
            }
        }

        int Init(ProxyConfig& config);
        int Run();
        const string& GetName()
        {
            return m_config.m_name;
        }
        int UpdateStatus(ServerCounter& counter)
        {
            time_t now = time(NULL);

            counter.m_start = m_saved_counter.m_end;
            counter.m_end = now;
            counter.m_recv_bytes = m_counter.m_recv_bytes - m_saved_counter.m_recv_bytes;
            counter.m_recv_pkts = m_counter.m_recv_pkts - m_saved_counter.m_recv_pkts;
            counter.m_send_bytes = m_counter.m_send_bytes - m_saved_counter.m_send_bytes;
            counter.m_send_pkts = m_counter.m_send_pkts - m_saved_counter.m_send_pkts;

            m_saved_counter = m_counter;
            m_saved_counter.m_start = counter.m_start;
            m_saved_counter.m_end = now;

            return 0;
        }
};

class UdpProxyServer
{
    private:
        std::vector<UdpProxy *> m_proxies;
        std::vector<boost::thread *> m_threads;
        ServerConfig m_config;

    public:
        int Init(ServerConfig& config);
        int Run();
};
#endif
