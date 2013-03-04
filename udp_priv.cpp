#include "udp_priv.h"

void StrStrip(string& str, char strip)
{
    size_t index = 0;
    size_t length;
    if(str.empty())
    {
        return;
    }
    length = str.length();
    for(index = length; index > 0 && str[index - 1] == strip; index --);
    str.erase(index, length - index);
    length = str.length();
    for(index = 0; index < length && str[index] == strip; index ++);
    str.erase(0, index);

    return;
}

int StrSplit(const string& str, char splitter, vector<string>& split)
{
    string::size_type pos_begin, pos;
    string tmp_str;

    pos_begin = 0;
    split.clear();
    while(pos_begin != string::npos)
    {
        pos = str.find(splitter, pos_begin + 1);
        if(pos != string::npos)
        {
            tmp_str = str.substr(pos_begin, pos - pos_begin);
            split.push_back(tmp_str);
        }
        else
        {
            tmp_str = str.substr(pos_begin);
            split.push_back(str.substr(pos_begin));
            break;
        }
        pos_begin = pos + 1;
    }
    return 0;
}

uint32_t Str2IP(string& str)
{
    uint32_t ip = 0;
    vector<string> ip_nums;

    StrSplit(str, '.', ip_nums);
    if(ip_nums.size() != 4)
    {
        return 0;
    }

    size_t index;
    for(index = 0; index < ip_nums.size(); index ++)
    {
        ip <<= 8;
        ip += atoi(ip_nums[index].c_str());
    }

    return ip;
}

int SetSocketFlag(int fd, int flags)
{
    int ec;

    ec = fcntl(fd, F_GETFL, 0);
    EC_CHECK_ERR_EXIT(ec);
    ec |= flags;
    ec = fcntl(fd, F_SETFL, ec);
    EC_CHECK_ERR_EXIT(ec);

err_exit:
    return ec;
}

bool IsMulticastIP(uint32_t ip)
{
    return (ip >> 28) == 0x0E;
}

