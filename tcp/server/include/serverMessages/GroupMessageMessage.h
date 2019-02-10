#ifndef SERVER_GROUP_MESSAGE_MESSAGE_H
#define SERVER_GROUP_MESSAGE_MESSAGE_H

#include "ServerMessage.h"

class GroupMessageMessage : public ServerMessage {

private:
    std::string user_name;
    std::string message;

public:
    explicit GroupMessageMessage(const std::string &user_name, const std::string &message);
    void WriteBody(int socket_fd) const override;
};

#endif
