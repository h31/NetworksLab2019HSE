//
// Created by mikhail on 16.02.19.
//

#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <thread>
#include <zconf.h>
#include <future>
#include <iostream>
#include <Server.h>
#include <fstream>


#include "Server.h"
#include "ClientHandler.h"

Server::Server(uint16_t portno, int max_number_of_pending_connections) : portno(portno),
max_number_of_pending_connections(max_number_of_pending_connections) {
    restore_previous_session();
}

void Server::run() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("ERROR setting socket option to SO_REUSEADDR");
    }
    struct sockaddr_in serv_addr{};
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    listen(sockfd, max_number_of_pending_connections);

    struct sockaddr_in cli_addr{};
    unsigned int clilen = sizeof(cli_addr);

    while (accept_clients) {
        int clientsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (clientsockfd < 0) {
            accept_clients = false;
        } else {
            std::cout << "New client with id = " << clientsockfd << "\n";
            client_handlers_with_tasks_mutex.lock();
            client_sockets.push_back(clientsockfd);
            client_handlers.push_back(new ClientHandler(clientsockfd, prime_numbers));
            client_handler_tasks.emplace_back(&ClientHandler::run, client_handlers.back());
            client_handlers_with_tasks_mutex.unlock();
        }
    }

    finish_client_handlers();
}

void Server::stop() const {
    shutdown(sockfd, SHUT_RDWR);
}

bool Server::shutdown_client(int client_id) {
    bool client_was_shutdown = false;
    client_handlers_with_tasks_mutex.lock();
    for (auto client_socket : client_sockets) {
        if (client_socket == client_id) {
            shutdown(client_socket, SHUT_RDWR);
            client_was_shutdown = true;
        }
    }
    client_handlers_with_tasks_mutex.unlock();
    return client_was_shutdown;
}

Server::~Server() {
    client_handlers_with_tasks_mutex.lock();
    for (auto &client_handler_task : client_handler_tasks) {
        client_handler_task.join();
    }
    for (auto client_socket : client_sockets) {
        close(client_socket);
    }
    client_handlers_with_tasks_mutex.unlock();
    close(sockfd);
    for (auto client_handler : client_handlers) {
        delete client_handler;
    }
    save_current_session();
}

void Server::finish_client_handlers() {
    client_handlers_with_tasks_mutex.lock();
    for (const auto &client_socket : client_sockets) {
        shutdown(client_socket, SHUT_RDWR);
    }
    client_handlers_with_tasks_mutex.unlock();
}

void Server::restore_previous_session() {
    std::ifstream file_to_restore(SESSION_FILE_NAME);
    if (file_to_restore.is_open()) {
        std::vector<int64_t> prime_numbers_to_add;
        while (file_to_restore.peek() != std::ifstream::traits_type::eof() && !file_to_restore.eof()) {
            int64_t prime_number;
            file_to_restore >> prime_number;
            prime_numbers_to_add.push_back(prime_number);
        }
        prime_numbers.add_prime_numbers(prime_numbers_to_add);
    }
    file_to_restore.close();
}

void Server::save_current_session() {
    std::ofstream file_to_restore(SESSION_FILE_NAME);
    if (file_to_restore.is_open()) {
        auto numbers = prime_numbers.get_all();
        for (size_t i = 0; i < numbers.size(); i++) {
            file_to_restore << numbers[i];
            if (i != numbers.size() - 1) {
                file_to_restore << " ";
            }
        }
    }
    file_to_restore.close();
}
