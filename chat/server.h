#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>

class Server
{
public:
    Server(const int _portNumber);

    void run();

private:
     struct sockaddr_in m_address;
     int m_socketFd;
};

#endif // SERVER_H
