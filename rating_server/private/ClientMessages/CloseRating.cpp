#include "ClientMessages/CloseRating.h"

bool ClientMessage::CloseRating::read_body(Socket &socket) {
  return socket.read_uint32(id);
}

bool ClientMessage::CloseRating::process(Server *server, Client *client) {
  return server->close_rating(id, client);
}
