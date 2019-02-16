#pragma once

#include <cstdint>
#include <netinet/in.h>
#include <thread>
#include <vector>

class server {
public:
    explicit server(uint16_t port);

    ~server();

    void start();

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

    std::vector<std::thread*> socket_pool{};
    std::vector<std::thread*> slow_ops_pool{};
};

