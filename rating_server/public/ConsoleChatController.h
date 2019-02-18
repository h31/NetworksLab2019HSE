#ifndef CHAT_CLIENT_CONSOLECHATCONTROLLER_H
#define CHAT_CLIENT_CONSOLECHATCONTROLLER_H


#include <mutex>

class ConsoleChatController {

  std::mutex output_mutex;

public:

private:
  void work_cycle();

  void get_user_input(std::string &result);
};

#endif //CHAT_CLIENT_CONSOLECHATCONTROLLER_H
