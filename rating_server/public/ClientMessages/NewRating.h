#ifndef RATING_SERVER_NEWRATING_H
#define RATING_SERVER_NEWRATING_H


#include "BaseMessage.h"


namespace ClientMessage {
  class NewRating : public BaseMessage {
  private:
    std::string rating_name;
    uint8_t cnt = 0;
  public:
    bool read_body(Socket &socket) override;

    bool process(Server *server, Client *client) override;
  };
}

#endif //RATING_SERVER_NEWRATING_H
