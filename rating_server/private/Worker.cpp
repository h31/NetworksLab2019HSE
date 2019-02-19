#include "Worker.h"

void Worker::operator()(Server *server, Client *client) {
  while (true) {
    ClientMessage::BaseMessage *message = nullptr;
    if (!(message = client->receive())) {
      std::cerr << "Message receive error" << std::endl;
      break;
    }
    bool result = message->process(server, client);
    delete message;
    if (!result) {
      std::cerr << "Message process error" << std::endl;
      break;
    }
  }
  if (!client->get_username()) server->remove_client(*client->get_username());
  delete client;
}
