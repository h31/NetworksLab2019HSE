#ifndef SERVER_LOGIN_MESSAGE_H
#define SERVER_LOGIN_MESSAGE_H

#include "ClientMessage.h"

class LoginMessage : public ClientMessage {

private:
    std::string user_name;

public:
    bool ReadBody(int socket_fd) override;
    bool Process(Server *server, Client *client) override;
};

#endif
