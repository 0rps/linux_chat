#include "client.h"
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>

#include <stdio.h>

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

    std::string nickname;

    while(nickname.length() == 0) {
        std::cout << "Input your nickname: ";
        std::cin >> nickname;
        std::cout << std::endl;
    }

    if (false == setNonblock(0) || false == setNonblock(m_socketFd)) {
        std::cerr << "Couldnot set noblocking descriptors" << std::endl;
        shutdown(m_socketFd, SHUT_RDWR);
        close(m_socketFd);
        return;
    }

    fd_set fdset;

    FD_ZERO(&fdset);
    FD_SET(0, &fdset);
    FD_SET(m_socketFd, &fdset);

    char buf[15];
    while(true) {

        select(m_socketFd+1, &fdset, NULL, NULL, NULL);

        if (FD_ISSET(0, &fdset)) {
            std::string input;
            std::getline(std::cin, input);

            Message msg(0x01, nickname, input);

            write(m_socketFd, (void*)msg.rawData(), msg.rawDataLength());
        }

        if (FD_ISSET(m_socketFd, &fdset)) {
            int count = read(m_socketFd, &buf, sizeof buf);

            if (count == 0) {
                std::cout << "Remote server closed connection " << std::endl;
                close(m_socketFd);
                return;
            } else {
                //std::cout << "Portion of data: " << count << std::endl;
            }
            handleMessage(buf, count);
        }

        FD_ZERO(&fdset);
        FD_SET(0, &fdset);
        FD_SET(m_socketFd, &fdset);

    }
}

void Client::handleMessage(const char *buffer, int msgLength)
{
    m_parser.addData(buffer, msgLength);
    while(m_parser.hasMessage()) {
        Message msg = m_parser.nextMessage();
        std::cout << msg.nickname() << ": " << msg.body() << std::endl;
    }
}
