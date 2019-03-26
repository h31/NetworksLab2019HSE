//
// Created by machine on 01-Feb-19.
//

#ifndef CHAT_CLIENT_COMMUNICATOR_H
#define CHAT_CLIENT_COMMUNICATOR_H

#include <string>
#include <functional>
#include <thread>
#include "Messages.h"
#include "socket/MySocket.h"


class Communicator {
  enum CommunicatorState : uint8_t {
    DISCONNECTED, CONNECTED
  };
private:
  MySocket *socket = nullptr;
  CommunicatorState state = DISCONNECTED;
  std::thread *listen_thread = nullptr;

public:
  Communicator(std::string &host, uint16_t port);

  ~Communicator();

  void send_message(Message &&message);

  void get_message_from_server(ServerMessage &result);

  bool start_receiving_messages(std::function<void(ServerMessage &)> &message_processing);

  void disconnect();

private:
  ServerMessage::ServerMessageType get_type_from_server();

  std::string *get_string_from_server();
};

#endif //CHAT_CLIENT_COMMUNICATOR_H
