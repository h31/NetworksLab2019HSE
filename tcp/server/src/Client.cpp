#include <sys/socket.h>
#include <mutex>

#include "clientMessages/LoginMessage.h"
#include "clientMessages/SingleMessageMessage.h"
#include "clientMessages/BroadcastMessageMessage.h"
#include "Client.h"

Client::Client(int socket_fd) : socket_fd(socket_fd), reader(socket_fd) {}

Client::~Client() {
    ShutdownSocket();
}

bool Client::IsLoggedIn() {
    std::unique_lock<std::recursive_mutex> lock(user_name_mtx);
    return !user_name.empty();
}

void Client::SetUserName(std::string &user_name) {
    std::unique_lock<std::recursive_mutex> lock(user_name_mtx);
    if (!IsLoggedIn()) {
        this->user_name = user_name;
    }
}

const std::string &Client::GetUserName() {
    return user_name;
}

void Client::ShutdownSocket() {
    static std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);
    if (socket_fd < 0) {
        return;
    }
    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);
    socket_fd = -1;
}

bool Client::Send(const ServerMessage &message) {
    return message.Write(socket_fd);
}

bool Client::Receive(ClientMessage **message) {
    ClientMessageType type;
    if (!reader.readBytes(reinterpret_cast<unsigned char *>(&type), sizeof(type))) {
        perror("Error on reading");
        return false;
    }
    switch (type) {
        case ClientMessageType::LOGIN:
            *message = new LoginMessage();
            break;
        case ClientMessageType::SINGLE_MESSAGE:
            *message = new SingleMessageMessage();
            break;
        case ClientMessageType::BROADCAST_MESSAGE:
            *message = new BroadcastMessageMessage();
            break;
        default:
            std::cerr << "Incorrect message type: " << (unsigned char) type << std::endl;
            return true;
    }
    return (*message)->ReadBody(reader);
}
