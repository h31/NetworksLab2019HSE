#ifndef RATING_SERVER_OPENRATING_H
#define RATING_SERVER_OPENRATING_H


#include "BaseMessage.h"

namespace ClientMessage {
  class OpenRating : public BaseMessage {
    uint32_t id;

    bool read_body(Socket &socket) override;

    bool process(Server *server, Client *client) override;
  };

}


#endif //RATING_SERVER_OPENRATING_H
