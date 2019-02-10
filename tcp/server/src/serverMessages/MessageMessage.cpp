#include <netinet/in.h>

#include "serverMessages/MessageMessage.h"

MessageMessage::MessageMessage(const std::string &user_name,
                               const std::string &message) : ServerMessage(ServerMessageType::MESSAGE) {
    this->user_name = user_name;
    this->message = message;
}

void MessageMessage::WriteBody(int socket_fd) const {
    WriteString(socket_fd, user_name);
    WriteString(socket_fd, message);
}
