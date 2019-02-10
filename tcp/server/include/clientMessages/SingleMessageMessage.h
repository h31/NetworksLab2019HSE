#ifndef SERVER_SINGLE_MESSAGE_MESSAGE_H
#define SERVER_SINGLE_MESSAGE_MESSAGE_H

#include "ClientMessage.h"

class SingleMessageMessage : public ClientMessage {

private:
    std::string user_name;
    std::string message;

public:
    bool ReadBody(int socket_fd) override;
    bool Process(Server *server, Client *client) override;
};

#endif
