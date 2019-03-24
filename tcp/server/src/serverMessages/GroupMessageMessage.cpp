#include "serverMessages/GroupMessageMessage.h"

GroupMessageMessage::GroupMessageMessage(const std::string &user_name, const std::string &message)
        : user_name(user_name),
          message(message),
          ServerMessage(ServerMessageType::GROUP_MESSAGE) {}

bool GroupMessageMessage::WriteBody(int socket_fd) const {
    return WriteString(socket_fd, user_name) && WriteString(socket_fd, message);
}
