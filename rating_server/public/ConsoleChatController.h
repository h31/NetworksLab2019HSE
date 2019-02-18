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

private:
  void work_cycle();

  void get_user_input(std::string &result);
};

#endif //CHAT_CLIENT_CONSOLECHATCONTROLLER_H
