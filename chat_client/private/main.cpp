#include <iostream>
#include "../public/Communicator.h"
#include "../public/ConsoleChatController.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "You should give two arguments: host port" << std::endl;
    return 1;
  }
  std::string host(argv[0]);
  std::string port(argv[1]);
  Communicator communicator(host, port);
  ConsoleChatController chat(communicator);
  chat.startWork();
  return 0;
}
