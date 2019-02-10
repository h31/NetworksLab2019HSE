#include "serverMessages/ErrorMessage.h"

ErrorMessage::ErrorMessage(const std::string &message) : ServerMessage(ServerMessageType::ERROR) {
    this->message = message;
}

bool ErrorMessage::WriteBody(int socket_fd) const {
    return WriteString(socket_fd, message);
}
