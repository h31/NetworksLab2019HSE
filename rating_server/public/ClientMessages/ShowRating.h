#ifndef RATING_SERVER_SHOWRATING_H
#define RATING_SERVER_SHOWRATING_H


#include "BaseMessage.h"

namespace ClientMessage {
  class ShowRating : public BaseMessage {
    uint32_t id;

    bool read_body(Socket &socket) override;

    bool process(Server *server, Client *client) override;
  };
}

#endif //RATING_SERVER_SHOWRATING_H
