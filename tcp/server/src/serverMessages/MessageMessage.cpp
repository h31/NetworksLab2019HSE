#include <netinet/in.h>

#include "serverMessages/MessageMessage.h"

MessageMessage::MessageMessage(const std::string &user_name,
                               const std::string &message) : user_name(user_name),
                                                             message(message),
                                                             ServerMessage(ServerMessageType::MESSAGE) {}

bool MessageMessage::WriteBody(int socket_fd) const {
    return WriteString(socket_fd, user_name) && WriteString(socket_fd, message);
}
