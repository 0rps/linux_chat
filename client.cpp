#include "client.h"
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>

#include <unistd.h>

Client::Client(const in_addr _addr, const int _port)
{
    bzero((char *) &m_serverAddr, sizeof(m_serverAddr));
    m_serverAddr.sin_family = AF_INET;
    bcopy((char *)&_addr,
          (char *)&m_serverAddr.sin_addr.s_addr,
          sizeof(_addr));
    m_serverAddr.sin_port = htons(_port);

}

void Client::run()
{
    m_socketFd = socket(AF_INET, SOCK_STREAM, 0);

    if (m_socketFd < 0) {
        std::cerr << "Couldn't create socket" << std::endl;
        return;
    }

    if (connect(m_socketFd, (struct sockaddr*)&m_serverAddr, sizeof(m_serverAddr)) < 0) {
        std::cerr << "Couldn't connect to server" << std::endl;
        return;
    }

    std::cout << "Connected to server" << std::endl << std::endl;


    char buf[256];
    char message[] = "Start message";

    send(m_socketFd, message, 14, 0);
    recv(m_socketFd, buf, 256, 0);

    std::cout << "Received: " << buf << std::endl;

    shutdown(m_socketFd, SHUT_RDWR);
    close(m_socketFd);
}
