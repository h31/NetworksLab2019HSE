#include "Client.h"

#include <ClientMessages/Connect.h>
#include <ClientMessages/NewRating.h>


Client::Client(int socket_fd) : socket(socket_fd) {
}

Client::~Client() {
  delete user_name;
  shutdown_socket();
}

void Client::shutdown_socket() {
  socket.shutdown();
}

void Client::operator()(Server *server) {
  while (true) {
    ClientMessage::BaseMessage *message = nullptr;
    if (!(message = receive())) {
      break;
    }
    bool result = message->process(server, this);
    delete message;
    if (!result) {
      break;
    }
  }
  if (!user_name) server->remove_client(*user_name);
  delete this;
}


ClientMessage::BaseMessage *Client::receive() {
  ClientMessage::MessageType type;
  if (socket.read_default(type)) {
    perror("Error while reading type");
    return nullptr;
  }

  switch (type) {
    case ClientMessage::CONNECT:
      return new ClientMessage::Connect();
    case ClientMessage::NEW_RATING:
      return new ClientMessage::NewRating();
    case ClientMessage::DELETE_RATING:
    case ClientMessage::OPEN_RATING:
    case ClientMessage::CLOSE_RATING:
    case ClientMessage::ADD_CHOICE:
    case ClientMessage::RATING_LIST:
    case ClientMessage::SHOW_RATING:
    case ClientMessage::VOTE_RATING:
    default:
      std::cerr << "Incorrect message type: " << (unsigned char) type << std::endl;
      return nullptr;
  }
}

void Client::set_username(std::string *name) {
  user_name = name;
}

Socket &Client::getSocket() {
  return socket;
}

bool Client::send_error(std::string &&message) {
  return socket.write_default(ServerMessage::ERROR) && socket.write_string(message);
}

bool Client::send_success() {
  return socket.write_default(ServerMessage::SUCCESS);
}
