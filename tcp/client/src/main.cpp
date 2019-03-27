#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include "Client.h"

int create_socket(char *server_ip, char *portno) {
    hostent *server = gethostbyname(server_ip);
    auto port = (uint16_t) strtol(portno, nullptr, 10);
    sockaddr_in server_addr{};
    std::fill((char *) &server_addr, (char *) &server_addr + sizeof(server_addr), 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    std::copy(server->h_addr, server->h_addr + (size_t) server->h_length, (char *) &server_addr.sin_addr.s_addr);
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
        perror("Error opening socket");
        return -1;
    }
    if (connect(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting");
        return -1;
    }
    return socketfd;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "usage: " << argv[0] << " ip-address port" << std::endl;
        return 0;
    }
    int socketfd = create_socket(argv[1], argv[2]);
    if (socketfd < 0) {
        return 0;
    }
    Client client(socketfd);
    client.run();
    close(socketfd);
    return 0;
}
