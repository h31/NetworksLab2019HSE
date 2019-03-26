//
// Created by machine on 02-Feb-19.
//

#ifndef CHAT_CLIENT_CONSOLECHATCONTROLLER_H
#define CHAT_CLIENT_CONSOLECHATCONTROLLER_H


#include <mutex>
#include "Communicator.h"

class ConsoleChatController {

  Communicator communicator;
  std::mutex output_mutex;

public:
  ConsoleChatController(Communicator &communicator) : communicator(communicator) {}

  void startWork();

  void login();

  void send_message();

  void send_broadcast();

  void disconnect();

  void print_message(ServerMessage &message);

private:
  void work_cycle();

  void get_user_input(std::string &result);
};

void static_print_message(ConsoleChatController *controller, ServerMessage &message);

#endif //CHAT_CLIENT_CONSOLECHATCONTROLLER_H
