#ifndef __UDP_PRIV_H__
#define __UDP_PRIV_H__
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <vector>

#define EC_CHECK_RETURN(ec) do { \
    if(ec < 0) \
    { \
        cout << "Failed ec: " << ec << " at file: " << __FILE__ << " line: " << __LINE__ \
            << " errno: " << errno << " " << strerror(errno) << endl; \
        return ec; \
    } \
} while(false);

#define EC_CHECK_ERR_EXIT(ec) do { \
    if(ec < 0) \
    { \
        cout << "Failed ec: " << ec << " at file: " << __FILE__ << " line: " << __LINE__ \
            << " errno: " << errno << strerror(errno) << endl; \
        goto err_exit; \
    } \
} while(false);

using namespace std;
extern void StrStrip(string& str, char strip);
extern int StrSplit(const string& str, char splitter, vector<string>& split);
extern uint32_t Str2IP(string& str);
extern int SetSocketFlag(int fd, int flags);
extern bool IsMulticastIP(uint32_t ip);

#endif
