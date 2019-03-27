#ifndef RATING_SERVER_CHOICERATING_H
#define RATING_SERVER_CHOICERATING_H


#include "BaseMessage.h"

namespace ClientMessage {
  class VoteRating : public BaseMessage {
    uint32_t id;
    uint8_t choice;

    bool read_body(Socket &socket) override;

    bool process(Server *server, Client *client) override;
  };
}

#endif //RATING_SERVER_CHOICERATING_H
