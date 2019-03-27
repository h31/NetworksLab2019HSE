#include <thread>
#include <boost/thread/csbl/vector.hpp>
#include "../include/tcp_server.h"

void TcpServer::StartServer(uint16_t port_number) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    sockaddr_in serv_addr = {0}, cli_addr = {0};

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_number);

    if (bind(socket_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }
    listen(socket_fd, 5);

    while (true) {

        unsigned int cli_len = sizeof(cli_addr);

        int new_socket_fd = accept(socket_fd, (struct sockaddr*) &cli_addr, &cli_len);

        if (new_socket_fd < 0) {
            perror("ERROR on accept");
            exit(1);
        }

        std::thread thread(&TcpServer::UserInteractionLoop, this, new_socket_fd);
        thread.detach();
    }
}
