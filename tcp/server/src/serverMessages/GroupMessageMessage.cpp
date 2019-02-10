#include "serverMessages/GroupMessageMessage.h"

GroupMessageMessage::GroupMessageMessage(const std::string &user_name, const std::string &message)
        : ServerMessage(ServerMessageType::GROUP_MESSAGE) {
    this->user_name = user_name;
    this->message = message;
}

bool GroupMessageMessage::WriteBody(int socket_fd) const {
    return WriteString(socket_fd, user_name) && WriteString(socket_fd, message);
}
