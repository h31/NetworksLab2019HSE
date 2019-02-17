#include "../include/BugTrackingServer.h"
#include <thread>
#include <netinet/in.h>
#include <strings.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

BugTrackingServer::BugTrackingServer(UserService* userService) : _userService(userService) {}

void BugTrackingServer::initServer(uint16_t port_number) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    sockaddr_in server_addr = {0}, cli_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_number);

    bzero((char *) &server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_number);

    if (bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }
    listen(socket_fd, 5);

    while (true) {
        unsigned int client = sizeof(cli_addr);
        int new_socket_fd = accept(socket_fd, (struct sockaddr*) &cli_addr, &client);
        if (new_socket_fd < 0) {
            perror("ERROR on accept");
            exit(1);
        }
        std::thread thread(&BugTrackingServer::process_client, this, new_socket_fd);
        thread.detach();
    }
}

void BugTrackingServer::process_client(int sock_fd) {
    std::cout << "New client connected on " << sock_fd << ".\n";
    Client client = Client(sock_fd, _userService);
    _clients_mutex.lock();
    _clients[sock_fd] = &client;
    _clients_mutex.unlock();
    uint32_t code;
    while (true) {
        if (!readInt32(sock_fd, code)) {
            std::cout << "Failed to process message from client " << sock_fd << ". Closing connection.\n";
            close(client);
            break;
        } else {
            if (code == 100) {
                authorize(client);
            } else if (!client.isAuthorized()) {
                // process unauthorized
            } else {
                if (code == 200) {

                } else if (code == 300) {

                } else if (code == 400) {

                } else if (code == 500) {

                } else if (code == 600) {

                } else if (code == 700) {
                    close(client);
                    break;
                } else {
                    writeInt32(sock_fd, 1);
                }
            }
        }
    }
}

bool BugTrackingServer::authorize(BugTrackingServer::Client& client) {
    int sock_fd = client.sock_fd;
    uint32_t id;
    readInt32(sock_fd, id);
    if (client.user.id != id) {
        // process double authorization
        return false;
    }
    if (client.authorize(id)) {
        writeInt32(sock_fd, 101);
        writeInt32(sock_fd, static_cast<uint32_t>(client.user.role));
        std::cout << "Client on socket " << sock_fd << " has authorized with id " << client.user.id;
        std::cout << " and role " << client.user.role << "\n";
        return true;
    } else {
        writeInt32(sock_fd, 102);
        writeInt32(sock_fd, id);
        std::cout << "Client on socket " << sock_fd << " has failed authorization\n";
        return false;
    }
}

bool BugTrackingServer::close(BugTrackingServer::Client &client) {
    _clients_mutex.lock();
    if (_clients.find(client.sock_fd) == _clients.end()) {
        _clients_mutex.unlock();
        std::cout << "Client with socket " << client.sock_fd << " is already disconnected\n";
        return false;
    } else {
        _clients.erase(client.sock_fd);
        _clients_mutex.unlock();
        std::cout << "Client with socket " << client.sock_fd << " has been disconnected\n";
        return true;
    }
}

/* READING FUNCTIONS */

bool BugTrackingServer::readInt32(int sock_fd, uint32_t& dst) {
    ssize_t n = read(sock_fd, &dst, sizeof(uint32_t));
    if (n < 0) {
        return false;
    }
    dst = ntohl(dst);
    return true;
}

bool BugTrackingServer::readString(int sock_fd, std::string& dst) {
    uint32_t length;
    if (!readInt32(sock_fd, length)) {
        return false;
    }
    char buf[length + 1];
    buf[length] = '\0';
    ssize_t n = read(sock_fd, buf, length);
    if (n < 0) {
        return false;
    }
    dst = std::string(buf);
    return true;
}

/* WRITING FUNCTIONS */

bool BugTrackingServer::writeInt32(int sock_fd, uint32_t number) {
    uint32_t value = htonl(number);
    return ::write(sock_fd, &value, sizeof(uint32_t)) >= 0;
}

bool BugTrackingServer::writeString(int sock_fd, std::string data) {
    auto length = static_cast<uint32_t>(data.length());
    if (!writeInt32(sock_fd, length)) {
        return false;
    }
    ssize_t written = ::write(sock_fd, data.c_str(), length);
    return written >= 0;
}

/*  CLIENT FUNCTIONS */

BugTrackingServer::Client::Client(
        int socket_fd,
        UserService* userService
) : sock_fd(socket_fd),  _userService(userService) {}

bool BugTrackingServer::Client::isAuthorized() {
    return user.isEmpty();
}

bool BugTrackingServer::Client::authorize(int id) {
    UserService::User newUser = _userService->getUser(id);
    if (newUser.isEmpty()) {
        return false;
    }
    user = newUser;
    return true;
}