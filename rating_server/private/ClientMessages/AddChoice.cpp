#include "ClientMessages/AddChoice.h"

bool ClientMessage::AddChoice::read_body(Socket &socket) {
  return socket.read_uint32(id) && socket.read_string(choice);
}

bool ClientMessage::AddChoice::process(Server *server, Client *client) {
  return server->add_choice(id, choice, client);
}
