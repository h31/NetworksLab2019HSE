#include "ClientMessages/VoteRating.h"


bool ClientMessage::VoteRating::read_body(Socket &socket) {
  return socket.read_uint32(id) && socket.read_default(choice);
}

bool ClientMessage::VoteRating::process(Server *server, Client *client) {
  return server->vote_rating(id, choice, client);
}
