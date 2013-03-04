#include <stdlib.h>
#include <fstream>
#include "udp_priv.h"
#include "config.h"

int ProxyConfig::Init(int argc, char **argv)
{
    if(argc != 7)
    {
        return -1;
    }
    m_name = argv[0];
    m_in_interface = argv[1];
    m_in_ip = argv[2];
    m_in_port = atoi(argv[3]);
    m_out_interface = argv[4];
    m_out_ip = argv[5];
    m_out_port = atoi(argv[6]);
    return 0;
}

int ProxyConfig::Init(string& name,
        string& in_interface,
        string& in_ip,
        int in_port,
        string& out_interface,
        string& out_ip,
        int out_port)
{
    m_name = name;
    m_in_interface = in_interface;
    m_in_ip = in_ip;
    m_in_port = in_port;
    m_out_interface = out_interface;
    m_out_ip = out_ip;
    m_out_port = out_port;

    return 0;
}

int ServerConfig::Init(int argc, char **argv)
{
    int ec;
    ProxyConfig proxy;
    ec = proxy.Init(argc, argv);
    EC_CHECK_RETURN(ec);
    m_proxies.push_back(proxy);
    return 0;
}

int ServerConfig::Init(const string& config_file)
{
#define MAX_LINE_LENGTH 1024
    char line_buf[MAX_LINE_LENGTH];
    string line;
    fstream fs(config_file.c_str(), ios_base::in);
    bool valid = false;
    bool new_proxy = false;
    ProxyConfig proxy;
    vector<string> split;

    if(!fs.is_open())
    {
        return -1;
    }
    while(fs.getline(line_buf, MAX_LINE_LENGTH))
    {
        line = line_buf;
        if(line.empty())
        {
            continue;
        }
        StrStrip(line, ' ');
        if(line[0] == '[' && line[line.length() - 1] == ']')
        {
            if(new_proxy)
            {
                m_proxies.push_back(proxy);
            }
            new_proxy = true;
            proxy.Reset();
            line.erase(line.length() - 1, 1);
            line.erase(0, 1);
            proxy.m_name = line;
        }
        else if(new_proxy)
        {
            StrSplit(line, '=', split);
            if(split.size() != 2)
            {
                // ignore...
                continue;
            }
            StrStrip(split[0], ' ');
            StrStrip(split[1], ' ');
            if(split[0] == "in_interface")
            {
                proxy.m_in_interface = split[1];
            }
            else if(split[0] == "in_ip")
            {
                proxy.m_in_ip = split[1];
            }
            else if(split[0] == "in_port")
            {
                proxy.m_in_port = atoi(split[1].c_str());
            }
            else if(split[0] == "out_interface")
            {
                proxy.m_out_interface = split[1];
            }
            else if(split[0] == "out_ip")
            {
                proxy.m_out_ip = split[1];
            }
            else if(split[0] == "out_port")
            {
                proxy.m_out_port = atoi(split[1].c_str());
            }
            else
            {
                // ignore...
            }
        }
        else
        {
            // ignore...
            continue;
        }
    }
    return 0;
}
