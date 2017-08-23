#ifndef SERVER_H
#define SERVER_H

#include <set>
#include <map>


#include "messageparser.h"

class Server
{
public:
    Server(const int _portNumber);

    void run();

private:
    bool runCycle();
    bool acceptConnections();
    bool handleClient(int _clientFd);

private:
    int m_portNumber;

    int m_listenFd;
    int m_epollFd;

    std::map<int, MessageParser*> m_clientStreams;
};

#endif // SERVER_H
