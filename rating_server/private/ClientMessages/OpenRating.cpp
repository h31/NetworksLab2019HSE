#include "ClientMessages/OpenRating.h"

bool ClientMessage::OpenRating::read_body(Socket &socket) {
  return socket.read_uint32(id);
}

bool ClientMessage::OpenRating::process(Server *server, Client *client) {
  return server->open_rating(id, client);
}
