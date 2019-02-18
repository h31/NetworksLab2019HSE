#ifndef RATING_SERVER_DELETERATING_H
#define RATING_SERVER_DELETERATING_H

#include <ClientMessages/BaseMessage.h>

namespace ClientMessage {
  class DeleteRating : public BaseMessage {
    uint32_t id;
    bool read_body(Socket &socket) override;

    bool process(Server *server, Client *client) override;
  };
}


#endif //RATING_SERVER_DELETERATING_H
