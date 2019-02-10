#include "serverMessages/ErrorMessage.h"

ErrorMessage::ErrorMessage(const std::string &message) : ServerMessage(ServerMessageType::ERROR) {
    this->message = message;
}

void ErrorMessage::WriteBody(int socket_fd) const {
    WriteString(socket_fd, message);
}
