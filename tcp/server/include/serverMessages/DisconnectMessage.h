#ifndef SERVER_DISCONNECT_MESSAGE_H
#define SERVER_DISCONNECT_MESSAGE_H

#include "ServerMessage.h"

class DisconnectMessage : public ServerMessage {

public:
    explicit DisconnectMessage();
};

#endif
