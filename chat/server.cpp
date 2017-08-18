#include "server.h"
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <cstring>

#include <unistd.h>

#define LISTEN_COUNT 50

Server::Server(const int _portNumber)
{
    bzero((char *) &m_address, sizeof(m_address));

    m_address.sin_family = AF_INET;
    m_address.sin_port = htons(_portNumber);
    m_address.sin_addr.s_addr = INADDR_ANY;
}

void Server::run()
{

    m_socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socketFd < 0) {
        std::cerr << "Couldn't create socket" << std::endl;
        return;
    }

    int reuseAddrOn = 1;
    if (setsockopt(m_socketFd, SOL_SOCKET, SO_REUSEADDR,(const char *) &reuseAddrOn, sizeof(reuseAddrOn)) < 0) {

        std::cerr << "Couldn't set REUSEADDR" << std::endl;
        close(m_socketFd);
        return;
    }

    if (bind(m_socketFd, (struct sockaddr *) &m_address, sizeof(m_address)) < 0) {
        std::cerr << "Couldn't bind address" << std::endl;
        return;
    }

    if ( listen(m_socketFd, LISTEN_COUNT) == -1 ) {
        std::cerr << "Listen error" << std::endl;
        return;
    }

    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);

    int clientSocket = accept(m_socketFd, (struct sockaddr *) &clientAddr, &clientAddrSize);

    char buf[256];

    int recvCount = recv(clientSocket, buf, 256, 0);
    std::cout << "Received: " << buf << std::endl;

    send(clientSocket, buf, recvCount, 0);

    shutdown(clientSocket, SHUT_RDWR);

    close(clientSocket);

    close(m_socketFd);

}
