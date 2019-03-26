#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <vector>
#include <thread>
#include <shared_mutex>
#include "product.h"


class server {
    private:
        static const int CONNECTION_QUEUE_SIZE = 5;
        const uint16_t PORT;
        int server_socket_fd;
        std::thread *main_thread = nullptr;
        std::vector<std::thread *> clients;
        std::vector<int> clients_sockets;
        std::vector<product> products;
        std::shared_mutex products_access;

        void client_accept_cycle(int server_socket_fd);

        void request_response_cycle(int client_socket_fd);

    public:
        explicit server(uint16_t port);

        ~server();

        void start();

        void stop();
};

#endif //SERVER_SERVER_H
