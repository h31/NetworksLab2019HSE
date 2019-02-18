#ifndef RATING_SERVER_RATINGLIST_H
#define RATING_SERVER_RATINGLIST_H


#include "BaseMessage.h"

namespace ClientMessage {
  class RatingList : public BaseMessage {
    bool process(Server *server, Client *client) override;
  };
}


#endif //RATING_SERVER_RATINGLIST_H
