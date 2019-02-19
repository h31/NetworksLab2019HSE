#include "Client.h"

#include <ClientMessages/Connect.h>
#include <ClientMessages/NewRating.h>
#include <ClientMessages/DeleteRating.h>
#include <ClientMessages/OpenRating.h>
#include <ClientMessages/CloseRating.h>
#include <ClientMessages/AddChoice.h>
#include <ClientMessages/RatingList.h>
#include <ClientMessages/ShowRating.h>
#include <ClientMessages/VoteRating.h>
#include <Client.h>


Client::Client(int socket_fd) : socket(socket_fd) {
}

Client::~Client() {
  delete user_name;
  std::cerr << "delete client" << std::endl;
  shutdown_socket();
}

void Client::shutdown_socket() {
  socket.shutdown();
}

ClientMessage::BaseMessage *Client::receive() {
  ClientMessage::MessageType type;
  if (!socket.read_default(type)) {
    perror("Error while reading type");
    return nullptr;
  }

  switch (type) {
    case ClientMessage::CONNECT:
      return new ClientMessage::Connect();
    case ClientMessage::NEW_RATING:
      return new ClientMessage::NewRating();
    case ClientMessage::DELETE_RATING:
      return new ClientMessage::DeleteRating();
    case ClientMessage::OPEN_RATING:
      return new ClientMessage::OpenRating();
    case ClientMessage::CLOSE_RATING:
      return new ClientMessage::CloseRating();
    case ClientMessage::ADD_CHOICE:
      return new ClientMessage::AddChoice();
    case ClientMessage::RATING_LIST:
      return new ClientMessage::RatingList();
    case ClientMessage::SHOW_RATING:
      return new ClientMessage::ShowRating();
    case ClientMessage::VOTE_RATING:
      return new ClientMessage::VoteRating();
    default:
      std::cerr << "Incorrect message type: " << (int) type << ":" << (unsigned char) type << std::endl;
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

std::string *Client::get_username() {
  return user_name;
}
