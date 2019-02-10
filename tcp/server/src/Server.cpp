#include <thread>
#include <Server.h>
#include <mutex>


#include "Acceptor.h"
#include "serverMessages/GroupMessageMessage.h"
#include "serverMessages/DisconnectMessage.h"
#include "Server.h"
#include "Client.h"

Server::Server(unsigned short port) {
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd < 0) {
        perror("Error opening socket");
        return;
    }
    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(server_socket_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("Error binding socket");
        ShutdownSocket();
        return;
    }
}

Server::~Server() {
    ShutdownSocket();
}

void Server::Run() {
    if (server_socket_fd < 0) {
        return;
    }
    listen(server_socket_fd, BACKLOG_SIZE);
    Acceptor acceptor;
    std::thread accept_thread(acceptor, this);
    while (true) {
        std::string input;
        std::cin >> input;
        if (input == EXIT_COMMAND) {
            break;
        }
        std::cout << "To exit enter: " << EXIT_COMMAND << std::endl;
    }
    ShutdownSocket();
    accept_thread.join();
}

bool Server::AddClient(const std::string &user_name, Client *client) {
    std::unique_lock<std::mutex> lock(active_clients_mtx);
    if (active_clients.count(user_name)) {
        return false;
    }
    active_clients[user_name] = client;
    return true;
}

void Server::RemoveClient(const std::string &user_name) {
    std::unique_lock<std::mutex> lock(active_clients_mtx);
    active_clients.erase(user_name);
}

void Server::SendToAll(const std::string &from, const GroupMessageMessage &message) {
    std::unique_lock<std::mutex> lock(active_clients_mtx);
    for (const auto &client : active_clients) {
        if (client.first != from) {
            client.second->Send(message);
        }
    }
}

bool Server::SendTo(const std::string &from, const std::string &to, const MessageMessage &message) {
    std::unique_lock<std::mutex> lock(active_clients_mtx);
    if (!active_clients.count(to)) {
        return false;
    }
    active_clients[to]->Send(message);
    return true;
}

void Server::ShutdownSocket() {
    static std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);
    if (server_socket_fd < 0) {
        return;
    }
    shutdown(server_socket_fd, SHUT_RDWR);
    server_socket_fd = -1;
}

void Server::ShutdownAllClients() {
    std::unique_lock<std::mutex> lock(active_clients_mtx);
    for (const auto &client : active_clients) {
        if (client.second->Send(DisconnectMessage())) {
            client.second->ShutdownSocket();
        }
    }
}

int Server::Accept() {
    struct sockaddr_in client_address{};
    unsigned int client_length = sizeof(client_address);
    return accept(server_socket_fd, (struct sockaddr *) &client_address, &client_length);
}
