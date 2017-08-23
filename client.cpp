#include "client.h"
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>

#include <sys/select.h>

#include <unistd.h>

#include "utils.h"

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

    setNonblock(0);
    setNonblock(m_socketFd);

    fd_set fdset;

    FD_ZERO(&fdset);
    FD_SET(0, &fdset);
    FD_SET(m_socketFd, &fdset);

    char buf[1024];
    char text[1024];
    int textlength = 0;

    while(true) {

        select(m_socketFd, &fdset, NULL, NULL, NULL);

        if (FD_ISSET(0, &fdset)) {
            int count = read(0, &buf, 1024);
            bool isSend = false;
            for (int i = 0; count; i++) {
                if (buf[i] == '\n') {
                    text[++textlength] = 0;
                    isSend = true;
                    break;
                }
                text[++textlength] = buf[i];
            }

            if (isSend) {
                write(m_socketFd, text, textlength);
            }
        }

        if (FD_ISSET(m_socketFd, &fdset)) {
            int count = read(m_socketFd, &buf, 1024);
            buf[count] = 0;
            std::cout << buf << std::endl;
        }

        FD_ZERO(&fdset);
        FD_SET(0, &fdset);
        FD_SET(m_socketFd, &fdset);

    }



//    char buf[256];
//    char message[] = "Start message";

//    send(m_socketFd, message, 14, 0);
//    recv(m_socketFd, buf, 256, 0);

//    std::cout << "Received: " << buf << std::endl;

//    shutdown(m_socketFd, SHUT_RDWR);
//    close(m_socketFd);
}
