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

    void client_handler(struct dctp_request_header request);
    dctp_response_header handle_new_request(struct dctp_request_header request);
    struct dctp_response_header handle_slow_function(
            struct dctp_request_header request,
            void(server::* process)(int, struct dctp_request_header)
            );

    void process_sqrt(int socket_descriptor, struct dctp_request_header request);

    void process_fact(int socket_descriptor, struct dctp_request_header request);
private:
    uint16_t port_number;
    int socket_descriptor;
    sockaddr_in server_addr{};
    sockaddr_in client_addr{};
    bool isInitialized = false;
    bool isTerminated = false;

    socket_int_pool slow_ops_pool;
    long_computation_response_pool computation_response_pool;
};

