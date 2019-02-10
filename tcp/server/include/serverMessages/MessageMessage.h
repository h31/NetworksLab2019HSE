#ifndef SERVER_MESSAGE_MESSAGE_H
#define SERVER_MESSAGE_MESSAGE_H

#include <string>

#include "ServerMessage.h"

class MessageMessage : public ServerMessage {

private:
    std::string user_name;
    std::string message;

public:
    MessageMessage(const std::string &user_name, const std::string &message);
    bool WriteBody(int socket_fd) const override;
};

#endif
