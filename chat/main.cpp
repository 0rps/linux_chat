#include <iostream>

#include <arpa/inet.h>

#include <client.h>
#include <server.h>

bool toIpAddress(const char* ip, struct in_addr &addr) {
    return inet_pton(AF_INET, ip, (void*)&addr) == 1;
}

bool isNumericPort(const char* port) {
    const char* start = port;
    while(*start != '\0') {
        if (!isdigit(*start))
            return false;
        start++;
    }
    return true;
}

void startClient(const char* serverAddr, const char* serverPort) {
    struct in_addr addr;

    if (false == toIpAddress(serverAddr, addr)) {
        std::cerr << "Ip address is not valid" << std::endl;
        return;
    }

    if (false == isNumericPort(serverPort)) {
        std::cerr << "Port is not valid" << std::endl;
        return;
    }

    Client client(addr, std::stoi(serverPort));
    client.run();
}

void startServer(const char *port) {
    if (false == isNumericPort(port)) {
        std::cerr << "Port is not valid" << std::endl;
        return;
    }

    Server server(std::stoi(port));
    server.run();
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr << "Argments count less than 3" << std::endl;
        return -1;
    }

    std::string app_type(argv[1]);
    if (app_type == "server") {
        startServer(argv[2]);
    } else if (app_type == "client") {
        if (argc < 4) {
            std::cerr << "For client start must be provided server addr and port number" << std::endl;
            return -1;
        }
        startClient(argv[2], argv[3]);
    } else {
        std::cerr << "Unknown client type" << std::endl;
        return -1;
    }

    return 0;
}

