#include "ClientMessages/NewRating.h"

using namespace ClientMessage;

bool NewRating::read_body(Socket &socket) {
  return socket.read_string(rating_name) && socket.read_default(cnt);
}

bool NewRating::process(Server *server, Client *client) {
  server->create_new_rating(rating_name, cnt, client);
  return false;
}
