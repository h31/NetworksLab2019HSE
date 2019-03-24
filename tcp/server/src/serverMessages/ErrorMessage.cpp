#include "serverMessages/ErrorMessage.h"

ErrorMessage::ErrorMessage(const std::string &message) : message(message),
                                                         ServerMessage(ServerMessageType::ERROR) {}

bool ErrorMessage::WriteBody(int socket_fd) const {
    return WriteString(socket_fd, message);
}
