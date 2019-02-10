#pragma once

#include <cstdint>
#include <netinet/in.h>
#include <iostream>
#include <strings.h>
#include <csignal>
#include <netdb.h>
#include <thread>
#include <unordered_set>
#include <request.hpp>
#include <unistd.h>

static volatile sig_atomic_t done = 0;

void term_handler(int signum);

class server {
public:
    explicit server(uint16_t port) : port(port) {
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);

        if (socket_fd < 0) {
            perror("Error opening socket");
        }

        /* Initialize socket structure */
        bzero((char *) &server_addr, sizeof(server_addr));


        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);

        if (bind(socket_fd, (sockaddr *) &server_addr, sizeof(server_addr))) {
            perror("Error binding socket.");
        }
    }

    ~server() {
        shutdown(socket_fd, SHUT_RD);
        close(socket_fd);
    }

    void start() {
        listen(socket_fd, 5);

        int client_socket_fd;
        unsigned int client_size = sizeof(client_addr);

        while (!done) {
            client_socket_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_size);

            if (client_socket_fd < 0) {
                perror("ERROR on accept");
                continue;
            }

            new_client(client_socket_fd);
        }
    }

private:
    void handle_client(int client_socket_fd) {
        pstp_request_header header;

        while (true) {

            ssize_t n = read(client_socket_fd, (char *) &header, sizeof(header));

            if (n < 0) {
                perror("ERROR reading from socket");
            }

            if (n == 0) {
                break;
            }

            switch (header.type) {
                case REGISTER:
                    break;
                case CHECK_LOGIN:
                    break;
            }
        }

        shutdown(client_socket_fd, SHUT_RD);
        close(client_socket_fd);
    }

    void handle_register(int client_socket_fd, pstp_request_header const &header) {
        // register
        std::cout << "Register";
    }

    void new_client(int client_socket_fd) {
        std::thread *thread = new std::thread(&server::handle_client, this, client_socket_fd);

    }

    uint16_t port = 1337;
    int socket_fd = 0;
    sockaddr_in server_addr = {};
    sockaddr_in client_addr = {};
    std::unordered_map<std::string, std::string>
};
