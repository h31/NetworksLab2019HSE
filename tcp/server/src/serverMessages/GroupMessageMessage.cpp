#include "serverMessages/GroupMessageMessage.h"

GroupMessageMessage::GroupMessageMessage(const std::string &user_name, const std::string &message)
        : ServerMessage(ServerMessageType::GROUP_MESSAGE) {
    this->user_name = user_name;
    this->message = message;
}

void GroupMessageMessage::WriteBody(int socket_fd) const {
    WriteString(socket_fd, user_name);
    WriteString(socket_fd, message);
}
