#ifndef RATING_SERVER_ADDCHOICE_H
#define RATING_SERVER_ADDCHOICE_H


#include "BaseMessage.h"

namespace ClientMessage {
  class AddChoice : public BaseMessage {
    uint32_t id;
    std::string choice;

    bool read_body(Socket &socket) override;

    bool process(Server *server, Client *client) override;
  };
}


#endif //RATING_SERVER_ADDCHOICE_H
