#include "ClientMessages/RatingList.h"

bool ClientMessage::RatingList::process(Server *server, Client *client) {
  return server->rating_list(client);
}
