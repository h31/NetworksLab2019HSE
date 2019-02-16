//
// Created by mikhail on 16.02.19.
//

#include <Server.h>
#include <bits/socket.h>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <thread>
#include <zconf.h>
#include <future>

#include "Server.h"

void check_connection_status(ssize_t read_bytes_number);

template<typename R>
bool is_future_ready(std::future<R> const &f) {
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

int64_t read_int64(int sockfd) {
    const size_t buffer_size = sizeof(int64_t);
    char buffer[buffer_size];
    bzero(buffer, buffer_size);
    check_connection_status(read(sockfd, buffer, buffer_size));
    int64_t intValue = buffer[0] | (buffer[1] << 8) | (buffer[2] << 16)|
            (buffer[3] << 24) | ((int64_t) buffer[4] << 32) | ((int64_t) buffer[5] << 40) |
            ((int64_t) buffer[6] << 48) | ((int64_t) buffer[7] << 56);
    return intValue;
}

int32_t read_int32(int sockfd) {
    const size_t buffer_size = 4;
    char buffer[buffer_size];
    bzero(buffer, buffer_size);
    check_connection_status(read(sockfd, buffer, buffer_size));
    int32_t intValue = buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
    return intValue;
}

int16_t read_int16(int sockfd) {
    const size_t buffer_size = 2;
    char buffer[buffer_size];
    bzero(buffer, buffer_size);
    check_connection_status(read(sockfd, buffer, buffer_size));
    int16_t intValue = buffer[0] | (buffer[1] << 8);
    return intValue;
}


void write_int64(int64_t value, int sockfd) {
    check_connection_status(write(sockfd, &value, sizeof(int64_t)));
}

void write_int8(int8_t value, int sockfd) {
    check_connection_status(write(sockfd, &value, sizeof(int8_t)));
}

void check_connection_status(ssize_t read_bytes_number) {
    if (read_bytes_number < 0) {
        std::string message = "ERROR communicating with socket.";
        perror(message.c_str());
        throw std::string(message);
    }

    if (read_bytes_number == 0) {
        throw "socket closed";
    }
}

int16_t Server::ClientHandler::read_message_end() const {
    return read_int16(sockfd);
}

void Server::run() {
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
        remove_finished_clients();

        int clientsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (clientsockfd < 0) {
            perror("ERROR on accept");
            accept_clients = false;
        } else {
            client_handlers_with_tasks.emplace_back(clientsockfd);
        }
    }

    finish_client_handlers();
}

void Server::remove_finished_clients() {
    for (auto it = client_handlers_with_tasks.begin(); it < client_handlers_with_tasks.end();) {
        if (it->is_ready()) {
            it = client_handlers_with_tasks.erase(it);
        } else {
            it++;
        }
    }
}

Server::Server(uint16_t portno, int max_number_of_pending_connections) : portno(portno),
sockfd(socket(AF_INET, SOCK_STREAM, 0)), max_number_of_pending_connections(max_number_of_pending_connections) {
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
}

Server::~Server() {
    close(sockfd);
}

void Server::finish_client_handlers() {
    for (const auto &client_handler_with_task : client_handlers_with_tasks) {
        client_handler_with_task.stop();
    }
}

void Server::stop() const {
    shutdown(sockfd, SHUT_RDWR);
}

bool Server::shutdown_client(int client_id) const {
    bool client_was_shutdown = false;
    for (const auto &client_handler_with_task : client_handlers_with_tasks) {
        if (client_handler_with_task.get_client_id() == client_id) {
            client_handler_with_task.stop();
            client_was_shutdown = true;
        }
    }
    return client_was_shutdown;
}

Server::ClientHandler::ClientHandler(int sockfd, PrimeNumbersConcurrent &prime_numbers) :
client_id(sockfd), sockfd(sockfd), prime_numbers(prime_numbers){}

Server::ClientHandler::~ClientHandler() {
    close(sockfd);
}

void Server::ClientHandler::stop() const {
    shutdown(sockfd, SHUT_RDWR);
}

int Server::ClientHandler::get_client_id() const {
    return client_id;
}

void Server::ClientHandler::run() const {
    bool is_stopped = false;
    while (!is_stopped) {
        try {
            int32_t command = read_command();

            switch (command) {
                case 0:
                    process_max_number();
                    break;
                case 1:
                    process_last_numbers();
                    break;
                case 2:
                    process_bound_for_calculation();
                    break;
                case 3:
                    process_add_prime_numbers();
                    break;
            }
        } catch (const char *exception_to_stop) {
            is_stopped = true;
        }
    }
}

int32_t Server::ClientHandler::read_command() const {
    return read_int32(sockfd);
}

void Server::ClientHandler::process_max_number() const {
    read_message_end();
    write_int64(prime_numbers.get_max(), sockfd);
    write_message_end();
}

void Server::ClientHandler::process_last_numbers() const {
    int64_t primes_number_to_return = read_int64(sockfd);
    read_message_end();
    std::vector<int64_t> numbers_to_return = prime_numbers.get_last((size_t) primes_number_to_return);
    write_int64(numbers_to_return.size(), sockfd);
    for (int64_t prime_number : numbers_to_return) {
        write_int64(prime_number, sockfd);
    }
    write_message_end();
}

void Server::ClientHandler::process_bound_for_calculation() const {
    read_int64(sockfd);
    read_message_end();
    write_int64(prime_numbers.get_bound_for_calculation(), sockfd);
    write_message_end();
}

void Server::ClientHandler::process_add_prime_numbers() const {
    int64_t n = read_int64(sockfd);
    std::vector<int64_t> numbers_to_add;
    for (int64_t i = 0; i < n; i++) {
        numbers_to_add.push_back(read_int64(sockfd));
    }
    read_message_end();
    write_int8(prime_numbers.add_prime_numbers(numbers_to_add), sockfd);
    write_message_end();
}

void Server::ClientHandler::write_message_end() const {
    write_int8('\\', sockfd);
    write_int8(0, sockfd);
}

bool Server::ClientHandlerWithTask::is_ready() const {
    return is_future_ready(client_handler_task);
}

Server::ClientHandlerWithTask::~ClientHandlerWithTask() {
    client_handler_task.get();
}

void Server::ClientHandlerWithTask::stop() const {
    client_handler.stop();
}

int Server::ClientHandlerWithTask::get_client_id() const {
    return client_handler.get_client_id();
}

Server::ClientHandlerWithTask::ClientHandlerWithTask(int clientsockfd, PrimeNumbersConcurrent &prime_numbers) :
client_handler(clientsockfd, prime_numbers), client_handler_task(std::async(std::launch::async, &ClientHandler::run, client_handler)) {}
