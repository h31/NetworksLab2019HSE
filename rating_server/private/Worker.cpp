#include "Worker.h"

void Worker::operator()(Server *server, Client *client) {
  while (true) {
    std::cout << "Listen message" << std::endl;

    ClientMessage::BaseMessage *message = nullptr;
    if (!(message = client->receive())) {
      std::cerr << "Message receive error" << std::endl;
      break;
    }
    if (!message->read_body(client->getSocket())) {
      continue;
    }
    std::cout << "Message received" << std::endl;
    bool result = message->process(server, client);
    delete message;
    if (!result) {
      std::cerr << "Message process error" << std::endl;
      break;
    }
    std::cout << "End while" << std::endl;
  }
  std::cout << "Message processed" << std::endl;
  if (!client->get_username()) server->remove_client(*client->get_username());
  delete client;
}
