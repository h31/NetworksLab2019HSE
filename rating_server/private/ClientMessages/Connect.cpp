#include <ClientMessages/Connect.h>

#include "ClientMessages/Connect.h"
using namespace ClientMessage;

bool Connect::read_body(Socket &socket) {
  return socket.read_string(*name);
}

bool Connect::process(Server *server, Client *client) {
  client->set_username(name);
  server->add_client(*name, client);
  name = nullptr;
  return true;
}

Connect::~Connect() {
  delete name;
}
