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
#include <response.hpp>
#include <unistd.h>
#include <unordered_map>
#include <mutex>
#include "wallet.hpp"

static volatile sig_atomic_t done = 0;

void term_handler(int signum);

class server {
public:
    explicit server(uint16_t port);

    ~server();

    void start();

private:
    void handle_client(int client_socket_fd);

    template<typename T>
    bool send_simple_response(int client_socket_fd, T &response) {
        ssize_t written = write(client_socket_fd, (char *) &response, sizeof(response));
        return written == sizeof(response);
    }

    template<typename T>
    bool send_serializable_response(int client_socket_fd, T &response) {
        size_t size = response.content_size() + sizeof(response.header);
        auto *buffer = new uint8_t[size]();
        response.serialize(buffer);
        return size == write(client_socket_fd, (char *) buffer, size);
    }

    bool handle_register(int client_socket_fd, pstp_request_header const &header);

    bool handle_check_login(int client_socket_fd, pstp_request_header const &header);

    bool handle_get_all_wallets(int client_socket_fd, pstp_request_header const &header);

    bool handle_account_info(int client_socket_fd, pstp_request_header const &header);

    bool handle_payment(int client_socket_fd, pstp_request_header const &header);

    bool handle_ask_for_payment(int client_socket_fd, pstp_request_header const &header);

    void new_client(int client_socket_fd);

    uint16_t port = 1337;
    int socket_fd = 0;
    sockaddr_in server_addr = {};
    sockaddr_in client_addr = {};
    std::unordered_map<std::string, wallet> wallets;
    std::mutex wallets_mutex;
    // std::vector<std::thread *> threads;
};
