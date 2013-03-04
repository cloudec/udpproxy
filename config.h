#ifndef __UDPPROXY_CONFIG_H__
#define __UDPPROXY_CONFIG_H__
#include <string>
using namespace std;

class ProxyConfig
{
    public:
        string m_name;
        string m_in_interface;
        string m_in_ip;
        int m_in_port;
        string m_out_interface;
        string m_out_ip;
        int m_out_port;
    public:
        ProxyConfig()
        {
            Reset();
        }
        int Init(int argc, char **argv);
        int Init(string& name, string& in_interface, string& in_ip, int in_port,
                string& out_interface, string& out_ip, int out_port);

        void Reset()
        {
            m_name = "";
            m_in_interface = "";
            m_in_ip = "";
            m_in_port = 0;
            m_out_interface = "";
            m_out_ip = "";
            m_out_port = 0;
        }
};

class ServerConfig
{
    public:
        vector<ProxyConfig> m_proxies;
    public:
        int Init(const string& config_file);
        int Init(int argc, char **argv);
};

#endif
