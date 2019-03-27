#include <netinet/in.h>
#include "ClientMessages/DeleteRating.h"

bool ClientMessage::DeleteRating::read_body(Socket &socket) {
  return socket.read_uint32(id);
}

bool ClientMessage::DeleteRating::process(Server *server, Client *client) {
  return server->delete_rating(id, client);
}
