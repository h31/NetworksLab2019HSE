#ifndef SERVER_ERROR_MESSAGE_H
#define SERVER_ERROR_MESSAGE_H

#include "ServerMessage.h"

class ErrorMessage : public ServerMessage {

private:
    std::string message;

public:
    explicit ErrorMessage(const std::string &message);

protected:
    bool WriteBody(int socket_fd) const override;
};

#endif
