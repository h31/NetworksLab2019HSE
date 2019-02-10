#include <netinet/in.h>

#include "serverMessages/ServerMessage.h"

ServerMessage::ServerMessage(ServerMessageType type) {
    this->type = type;
}

void ServerMessage::WriteString(int socket_fd, const std::string &value) {
    write(socket_fd, value.c_str(), value.size() + 1);
}

void ServerMessage::Write(int socket_fd) const {
    write(socket_fd, &type, sizeof(type));
    WriteBody(socket_fd);
}
