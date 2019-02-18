#ifndef RATING_SERVER_CONNECT_H
#define RATING_SERVER_CONNECT_H


#include "BaseMessage.h"

namespace ClientMessage {
  class Connect : public BaseMessage {
  private:
    std::string *name = new std::string();
  public:
    Connect() = default;

    bool read_body(Socket &socket) override;

    bool process(Server *server, Client *client) override;

    ~Connect() override;
  };
}

#endif //RATING_SERVER_CONNECT_H
