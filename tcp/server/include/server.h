#pragma once

#include <cstdint>
#include <netinet/in.h>
#include <thread>
#include <vector>
#include "pools.h"

class server {
public:
    explicit server(uint16_t port);

    ~server();

    void wait_for_clients();

    void client_handler(int socket_descriptor);

    void process_sqrt(int socket_descriptor, struct dctp_request_header request);

    void process_fact(int socket_descriptor, struct dctp_request_header request);
private:
    uint16_t port_number;
    int socket_descriptor;
    sockaddr_in server_addr{};
    sockaddr_in client_addr{};
    bool isInitialized = false;
    bool isTerminated = false;

    socket_pool client_socket_pool;
    socket_int_pool slow_ops_pool;
};

