//
// Created by machine on 02-Feb-19.
//

#include <thread>
#include "../public/Communicator.h"

Communicator::Communicator(std::string &host, std::string &port) {
  socket = MySocket::create_socket(host, port);
  state = CONNECTED;
}

void Communicator::send_message(ClientMessage &&message) {
  socket->send(&message.type, sizeof(message.type));
  //TODO
}

void Communicator::get_message_from_server(ServerMessage &result) {
  result.type = get_type_from_server();
  //TODO
}

ServerMessage::ServerMessageType Communicator::get_type_from_server() {
  ServerMessage::ServerMessageType type;
  int n = socket->receive(&type, sizeof(ServerMessage::type));
  if (n != sizeof(ServerMessage::type)) {
    perror("Error in message type");
    exit(1);
  }
  return static_cast<ServerMessage::ServerMessageType>(type);
}

std::string *Communicator::get_string_from_server() {
  char c;
  auto *result = new std::string;
  do {
    int n = socket->receive(&c, 1);
    if (n != 1) {
      perror("Error in message string");
      exit(1);
    }
    *result += c;
  } while (c != 0);
  return result;
}

bool Communicator::start_receiving_messages(std::function<void(ServerMessage &)> &message_processing) {
  if (listen_thread != nullptr)
    return false;

  listen_thread = new std::thread(
      [this, message_processing] {
        while (state != DISCONNECTED) {
          ServerMessage message;
          get_message_from_server(message);
          message_processing(message);
        }
      }
  );
  return true;
}

void Communicator::disconnect() {
  state = DISCONNECTED;
  socket->close();
  if (listen_thread != nullptr) listen_thread->join();
}

Communicator::~Communicator() {
  if (listen_thread != nullptr) {
    free(listen_thread);
    listen_thread = nullptr;
  }

  free(socket);
}
