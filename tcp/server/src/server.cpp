#include "../include/server.h"


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void server::client_accept_cycle(int server_socket_fd) {
    while (true) {
        clients_mutex.lock();
        boost::this_thread::interruption_point();

        struct sockaddr_in client_address{};
        unsigned int clilen = sizeof(client_address);
        int client_socket_fd = accept(server_socket_fd, (struct sockaddr *) &client_address, &clilen);
        if (client_socket_fd >= 0) {
            auto *client_thread = new boost::thread(&server::request_response_cycle, this);
            clients.push_back(client_thread);
        }

        clients_mutex.unlock();
    }
}
#pragma clang diagnostic pop

void server::request_response_cycle() {

}

server::server(uint16_t port) : PORT(port) {}

void server::start() {
    int server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd < 0) {
        throw socket_opening_exception();
    }

    struct sockaddr_in server_address{};
    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket_fd, (sockaddr *) &server_address, sizeof(server_address)) < 0) {
        throw binding_exception();
    }
    listen(server_socket_fd, CONNECTION_QUEUE_SIZE);

    main_thread = new boost::thread(&server::client_accept_cycle, this, server_socket_fd);
}

void server::stop() {
    clients_mutex.lock();
    main_thread->interrupt();
    for (auto client : clients) {
        client->interrupt();
    }
    clients_mutex.unlock();

    main_thread->join();
    for (auto client : clients) {
        client->join();
    }

    for (auto client : clients) {
        delete client;
    }
    delete main_thread;
}
