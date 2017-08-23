#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>
#include "messageparser.h"

class Client
{
public:
    Client(const struct in_addr _addr, const int _port);

    void run();
    void handleMessage(const char* buffer, int msgLength);
private:
    struct sockaddr_in m_serverAddr;
    int m_socketFd;
    MessageParser m_parser;
};

#endif // CLIENT_H
