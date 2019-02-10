#include <thread>

#include "Acceptor.h"
#include "Server.h"
#include "serverMessages/GroupMessageMessage.h"
#include "serverMessages/DisconnectMessage.h"
#include "Server.h"

Server::Server() {
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd < 0) {
        perror("Error opening socket");
        return;
    }
    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_socket_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("Error binding socket");
        ShutdownSocket();
        return;
    }
}

Server::~Server() {
    ShutdownSocket();
}

int Server::GetSocket() {
    return server_socket_fd;
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
    JoinAllClients();
}

void Server::JoinAllClients() {
    for (const auto &client : active_clients) {
        client.second->Join();
    }
}

bool Server::AddClient(const std::string &user_name, Client *client) {
    if (active_clients.count(user_name)) {
        return false;
    }
    active_clients[user_name] = client;
    return true;
}

void Server::SendToAll(const std::string &from, const GroupMessageMessage &message) {
    for (const auto &client : active_clients) {
        if (client.first != from) {
            message.Write(client.second->GetSocket());
        }
    }
}

bool Server::SendTo(const std::string &from, const std::string &to, const MessageMessage &message) {
    if (!active_clients.count(to)) {
        return false;
    }
    message.Write(active_clients[to]->GetSocket());
    return true;
}

void Server::ShutdownSocket() {
    if (server_socket_fd < 0) {
        return;
    }
    shutdown(server_socket_fd, SHUT_RDWR);
    server_socket_fd = -1;
}

void Server::ShutdownAllClients() {
    for (const auto &client : active_clients) {
        DisconnectMessage().Write(client.second->GetSocket());
        client.second->ShutdownSocket();
    }
}
