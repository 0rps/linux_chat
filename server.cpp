#include "server.h"
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <fcntl.h>
#include <sys/epoll.h>

#include <cstring>
#include <unistd.h>

#include "utils.h"

#define LISTEN_COUNT 50

Server::Server(const int _portNumber): m_portNumber(_portNumber)
{

}

void Server::run()
{

    struct sockaddr_in serverAddress;

    bzero((char *) &serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(m_portNumber);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

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

    if (bind(m_listenFd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Couldn't bind address" << std::endl;
        close(m_listenFd);
        return;
    }

    if ( listen(m_listenFd, SOMAXCONN) == -1 ) {
        std::cerr << "Listen error" << std::endl;
        close(m_listenFd);
        return;
    }

    struct epoll_event event;

    if ( (m_epollFd = epoll_create1(0)) == -1) {
        std::cerr << "Epoll create error for listen socket" << std::endl;
        close(m_listenFd);
        return;
    }
    event.data.fd = m_epollFd;
    event.events = EPOLLIN || EPOLLET;

    if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, m_listenFd, &event)) {
        std::cerr << "epoll ctl error for listen socket" << std::endl;
        close(m_listenFd);
        return;
    }

    std::cout << "Server is listening" << std::endl;

    if (runCycle() == false) {
        close(m_listenFd);
        abort();
    }

    close(m_listenFd);



//    struct sockaddr_in clientAddr;
//    socklen_t clientAddrSize = sizeof(clientAddr);

//    int clientSocket = accept(m_listenFd, (struct sockaddr *) &clientAddr, &clientAddrSize);

//    char buf[512];

//    int recvCount = recv(clientSocket, buf, 256, 0);
//    std::cout << "Received: " << buf << std::endl;

//    send(clientSocket, buf, recvCount, 0);

//    shutdown(clientSocket, SHUT_RDWR);

//    close(clientSocket);

//    close(m_listenFd);

}

bool Server::runCycle()
{
    struct epoll_event *events;
    events = (epoll_event*)calloc(LISTEN_COUNT, sizeof(epoll_event));

    while(true) {
        int count = epoll_wait(m_epollFd, events, LISTEN_COUNT, -1);
        for (int i = 0; i < count; i++) {
            int currentFd = events[i].data.fd;
            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP) ||
                (!(events[i].events & EPOLLIN))) {
                    /* An error has occured on this fd, or the socket is not
                       ready for reading (why were we notified then?) */
                std::cerr << "Some error occured with client fd in epoll" << std::endl;
                close (currentFd);
                continue;
            }
            bool isError = false;
            if (m_listenFd == currentFd) {
                isError = acceptConnections();
            } else {
                isError = handleClient(currentFd);
            }

            if (isError) {
                free(events);
                return false;
            }
        }
    }

    return true;
}

bool Server::acceptConnections()
{
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    while(true) {
        struct sockaddr in_addr;
        socklen_t in_len = sizeof in_addr;

        int clientFd;

        if ((clientFd = accept (m_listenFd, &in_addr, &in_len)) == -1)
        {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                return true;
            } else {
                std::cerr << "Accept error " << std::endl;
                return false;
            }
        }

        if (false == setNonblock(clientFd)) {
            shutdown(clientFd, SHUT_RDWR);
            close(clientFd);
            return true;
        }

        event.data.fd = clientFd;
        if ( epoll_ctl (m_epollFd, EPOLL_CTL_ADD, clientFd, &event) == -1) {
            std::cerr << "Add client socket to epoll error" << std::endl;
            return false;
        }

        m_clientStreams.insert(std::pair<int, MessageParser*>(clientFd, NULL));

        std::cout << "Add new connection: " << clientFd  << std::endl;
    }
}

bool Server::handleClient(int _clientFd)
{
    static char buf[512];
    static int bufsize = sizeof buf;

    bool isClose = false;

    int count = read(_clientFd, buf, sizeof buf);
    if (count == -1) {
        // read all
        if (errno != EAGAIN) {
            std::cout << "Read all" << std::endl;
            isClose = true;
        }
    } else if (count == 0) {
        // close connection
        isClose = true;

    } else {
        auto it = m_clientStreams.begin();
        std::cout << "read " << count << " bytes from " << _clientFd << std::endl;

        while(it != m_clientStreams.end()) {
            if (it->first != _clientFd) {
                write(it->first, buf, count);
            }
            it++;
        }
    }

    if (isClose) {
        std::cout << "Close connection: " << _clientFd  << std::endl;
        close(_clientFd);
        m_clientStreams.erase(_clientFd);
    }

    return true;
}
