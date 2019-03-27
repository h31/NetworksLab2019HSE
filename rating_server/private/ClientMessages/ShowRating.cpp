#include "ClientMessages/ShowRating.h"

bool ClientMessage::ShowRating::read_body(Socket &socket) {
  return socket.read_uint32(id);
}

bool ClientMessage::ShowRating::process(Server *server, Client *client) {
  return server->show_rating(id, client);
}
