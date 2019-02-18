#ifndef RATING_SERVER_CLOSERATING_H
#define RATING_SERVER_CLOSERATING_H


#include "BaseMessage.h"

namespace ClientMessage {
  class CloseRating : public BaseMessage {
    uint32_t id;
    bool read_body(Socket &socket) override;

    bool process(Server *server, Client *client) override;
  };
}


#endif //RATING_SERVER_CLOSERATING_H
