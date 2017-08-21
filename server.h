#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>

#include <set>
#include <map>


#include "messageparser.h"

class Server
{
public:
    Server(const int _portNumber);

    void run();

private:
     struct sockaddr_in m_serverAddress;

     int m_listenFd;
     int m_epollFd;

     std::map<int, MessageParser*> m_clientStreams;
     std::map<int, std::string> m_clients;
};

#endif // SERVER_H
