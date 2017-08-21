#include "server.h"
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <cstring>

#include <unistd.h>

#include "utils.h"

#define LISTEN_COUNT 50

Server::Server(const int _portNumber)
{
    bzero((char *) &m_serverAddress, sizeof(m_serverAddress));

    m_serverAddress.sin_family = AF_INET;
    m_serverAddress.sin_port = htons(_portNumber);
    m_serverAddress.sin_addr.s_addr = INADDR_ANY;
}

void Server::run()
{

    m_listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listenFd < 0) {
        std::cerr << "Couldn't create socket" << std::endl;
        return;
    }

    int reuseAddrOn = 1;
    if (setsockopt( m_listenFd, SOL_SOCKET, SO_REUSEADDR,(const char *) &reuseAddrOn, sizeof(reuseAddrOn) ) < 0) {

        std::cerr << "Couldn't set REUSEADDR" << std::endl;
        close(m_listenFd);
        return;
    }

    if (false == setNonblock(m_listenFd)) {
        std::cerr << "Couldn't set nonblocking listen socket" << std::endl;
        close(m_listenFd);
        return;
    }

    if (bind(m_listenFd, (struct sockaddr *) &m_serverAddress, sizeof(m_serverAddress)) < 0) {
        std::cerr << "Couldn't bind address" << std::endl;
        return;
    }

    if ( listen(m_listenFd, SOMAXCONN) == -1 ) {
        std::cerr << "Listen error" << std::endl;
        return;
    }

    std::cout << "Server is listening" << std::endl;

    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);

    int clientSocket = accept(m_listenFd, (struct sockaddr *) &clientAddr, &clientAddrSize);

    char buf[512];

    int recvCount = recv(clientSocket, buf, 256, 0);
    std::cout << "Received: " << buf << std::endl;

    send(clientSocket, buf, recvCount, 0);

    shutdown(clientSocket, SHUT_RDWR);

    close(clientSocket);

    close(m_listenFd);

}
