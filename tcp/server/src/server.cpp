#include "../include/server.h"
#include "../include/socket_io.h"


void server::client_accept_cycle(int server_socket_fd) {
    while (true) {
        clients_vector_access.lock();
        try {
            boost::this_thread::interruption_point();
        } catch (boost::thread_interrupted) {
            clients_vector_access.unlock();
            break;
        }

        struct sockaddr_in client_address{};
        unsigned int clilen = sizeof(client_address);
        int client_socket_fd = accept(server_socket_fd, (struct sockaddr *) &client_address, &clilen);
        if (client_socket_fd >= 0) {
            auto *client_thread = new boost::thread(&server::request_response_cycle, this, client_socket_fd);
            clients.push_back(client_thread);
        }

        clients_vector_access.unlock();
    }
}

void server::request_response_cycle(int client_socket_fd) {
    auto io = socket_io(client_socket_fd);

    try {
        while (true) {
            boost::this_thread::interruption_point();

            int command = io.read_int();
            switch (command) {
                case 1: {
                    size_t size = io.read_size_t();
                    std::string name = io.read_string(size);
                    size_t price = io.read_size_t();
                    size_t amount = io.read_size_t();
                    boost::upgrade_lock<boost::shared_mutex> lock(products_access);
                    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
                    products.emplace_back(name, price, amount);
                }
                    break;

                case 2: {
                    boost::shared_lock<boost::shared_mutex> lock(products_access);
                    io.write_size_t(products.size());
                    for (int i = 0; i < products.size(); i++) {
                        io.write_int(i);
                        io.write_size_t(products[i].name.length());
                        io.write_string(products[i].name);
                        io.write_size_t(products[i].price);
                        io.write_size_t(products[i].amount);
                    }
                }
                    break;

                case 3: {
                    int id = io.read_int();
                    boost::shared_lock<boost::shared_mutex> lock(products_access);
                    if (id < 0 || id >= products.size()) {
                        io.write_int(-1);
                    } else if(products[id].amount == 0) {
                        io.write_int(0);
                    } else {
                        products[id].amount--;
                        io.write_int(1);
                    }
                }
                    break;

                case 4: {
                    close(client_socket_fd);
                    return;
                }
            }
        }
    } catch (...) {
        close(client_socket_fd);
    }
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
    clients_vector_access.lock();
    main_thread->interrupt();
    for (auto client : clients) {
        client->interrupt();
    }
    clients_vector_access.unlock();

    main_thread->join();
    for (auto client : clients) {
        client->join();
    }

    for (auto client : clients) {
        delete client;
    }
    delete main_thread;
}
