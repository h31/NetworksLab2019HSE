#include <netinet/in.h>

#include "serverMessages/ServerMessage.h"

ServerMessage::ServerMessage(ServerMessageType type) {
    this->type = type;
}

bool ServerMessage::WriteString(int socket_fd, const std::string &value) {
    return write(socket_fd, value.c_str(), value.size() + 1) == value.size() + 1;
}

bool ServerMessage::Write(int socket_fd) const {
    if (write(socket_fd, &type, sizeof(type)) < sizeof(type)) {
        return false;
    }
    return WriteBody(socket_fd);
}
