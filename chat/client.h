#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>

class Client
{
public:
    Client(const struct in_addr _addr, const int _port);

    void run();

private:
    struct sockaddr_in m_serverAddr;
    int m_socketFd;
};

#endif // CLIENT_H
