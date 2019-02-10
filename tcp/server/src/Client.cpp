#include <sys/socket.h>

#include "Client.h"

Client::Client(int socket_fd) {
    this->socket_fd = socket_fd;
}

Client::~Client() {
    shutdown(socket_fd, SHUT_RDWR);
}

int Client::GetSocket() const {
    return socket_fd;
}

void Client::SetThread(std::thread *thread) {
    this->thread = thread;
}

void Client::Join() {
    thread->join();
}

bool Client::IsLoggedIn() {
    return !user_name.empty();
}

void Client::SetUserName(std::string &user_name) {
    if (!IsLoggedIn()) {
        this->user_name = user_name;
    }
}

const std::string &Client::GetUserName() {
    return user_name;
}

void Client::ShutdownSocket() {
    if (socket_fd < 0) {
        return;
    }
    shutdown(socket_fd, SHUT_RDWR);
    socket_fd = -1;
}
