#pragma onc
#include<arpa/inet.h>
 
class InetAddress{
    public:
        struct sockaddr_in addr;
        socklen_t addr_len;
        InetAddress();
        InetAddress(const char*,uint16_t);
        ~InetAddress();
};