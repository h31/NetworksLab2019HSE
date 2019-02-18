//
// Created by machine on 02-Feb-19.
//

#include <iostream>
#include "../public/ConsoleChatController.h"

void ConsoleChatController::get_user_input(std::string &result) {
  do {
    std::getline(std::cin, result);
  } while (result.length() == 0);
}

void ConsoleChatController::work_cycle() {
  while (true) {
    std::cout << "type your command" << std::endl;
    char command;
    std::cin >> command;
    switch (command) {
      case 'p':
        break;
      case 'b':
        break;
      case 'q':
        return;
      default:
        std::cout << "Type p (private), b (broadcast) or q (quit)" << std::endl;
    }
  }
}