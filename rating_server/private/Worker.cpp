#include "Worker.h"

void Worker::operator()(Server *server, Client *client) {
  while (true) {
    std::cout << ">>>>" << std::endl;

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
    std::cout << "Message processed" << std::endl;
    delete message;
    if (!result) {
      std::cerr << "Message process error" << std::endl;
      break;
    }
    std::cout << "<<<<" << std::endl;
  }
  std::cout << "break out" << std::endl;
  if (!client->get_username()) server->remove_client(*client->get_username());
  delete client;
}
