#include <netinet/in.h>
#include <string.h>
#include <zconf.h>
#include "../include/server.h"
#include "../include/socket_io.h"
#include "../include/socket_opening_exception.h"
#include "../include/binding_exception.h"
#include "../include/query_type.h"


void server::client_accept_cycle(int server_socket_fd) {
    while (true) {
        struct sockaddr_in client_address{};

        unsigned int client = sizeof(client_address);
        int client_socket_fd = accept(server_socket_fd, (struct sockaddr *) &client_address, &client);
        if (client_socket_fd >= 0) {
            clients_sockets.push_back(client_socket_fd);
            auto *client_thread = new std::thread(&server::request_response_cycle, this, client_socket_fd);
            clients.push_back(client_thread);
        } else {
            for (int client_socket : clients_sockets) {
                shutdown(client_socket, SHUT_RDWR);
                close(client_socket);
            }
            break;
        }
    }
}

void server::request_response_cycle(int client_socket_fd) {
    auto io = socket_io(client_socket_fd);

    try {
        while (true) {
            int query = io.read<int32_t>();
            switch (static_cast<query_type>(query)) {
                case ADD_PRODUCT: {
                    auto size = io.read<uint64_t>();
                    std::string name = io.read_string(size);
                    auto price = io.read<uint64_t>();
                    auto amount = io.read<uint64_t>();
                    std::unique_lock<std::shared_mutex> lock(products_access);
                    products.emplace_back(name, price, amount);
                }
                    break;

                case LIST_PRODUCTS: {
                    std::shared_lock<std::shared_mutex> lock(products_access);
                    io.write<uint64_t>(products.size());
                    for (int i = 0; i < products.size(); i++) {
                        io.write<int32_t>(i);
                        io.write<uint64_t>(products[i].name.length());
                        io.write(products[i].name);
                        io.write<uint64_t>(products[i].price);
                        io.write<uint64_t>(products[i].amount);
                    }
                }
                    break;

                case BUY_PRODUCT: {
                    int id = io.read<int32_t>();
                    std::shared_lock<std::shared_mutex> lock(products_access);
                    if (id < 0 || id >= products.size()) {
                        io.write<int32_t>(-1);
                    } else if(products[id].amount == 0) {
                        io.write<int32_t>(0);
                    } else {
                        products[id].amount--;
                        io.write<int32_t>(1);
                    }
                }
                    break;

                default:break;
            }
        }
    } catch (...) {
        close(client_socket_fd);
    }
}

server::server(uint16_t port) : PORT(port) {}

server::~server() {
    stop();
}

void server::start() {
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
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

    main_thread = new std::thread(&server::client_accept_cycle, this, server_socket_fd);
}

void server::stop() {
    if (server_socket_fd < 0) {
        return;
    }

    shutdown(server_socket_fd, SHUT_RDWR);
    main_thread->join();
    for (auto client : clients) {
        client->join();
    }

    for (auto client : clients) {
        delete client;
    }

    close(server_socket_fd);
    server_socket_fd = -1;
    delete main_thread;
}
