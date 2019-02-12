//
// Created by machine on 02-Feb-19.
//

#include <iostream>
#include "../public/ConsoleChatController.h"

void ConsoleChatController::login() {
  std::string username;

  while (true) {
    std::cout << "Enter your username: ";
    std::cin >> username;

    communicator.send_message(Message(Message::LOG_IN, &username));
    ServerMessage message;
    communicator.get_message_from_server(message);
    print_message(message);

    if (message.type == ServerMessage::SUCCESSFUL_CONNECT) {
      break;
    } else {
      std::cout << "Please, try again:" << std::endl;
    }
  }
}

void ConsoleChatController::send_message() {
  output_mutex.lock();
  std::cout << "type name of recipient: ";
  std::string user_to;
  get_user_input(user_to);

  std::string message_text;
  std::cout << "type your message: ";
  get_user_input(message_text);

  communicator.send_message(Message(Message::SINGLE_MESSAGE, &user_to, &message_text));
  output_mutex.unlock();
}

void ConsoleChatController::send_broadcast() {
  output_mutex.lock();
  std::string message_text;
  std::cout << "type your message: ";
  get_user_input(message_text);

  communicator.send_message(Message(Message::BROADCAST_MESSAGE, &message_text));
  output_mutex.unlock();
}

void ConsoleChatController::print_message(ServerMessage &message) {
  output_mutex.lock();
  switch (message.type) {
    case ServerMessage::BROADCAST_MESSAGE:
      std::cout << message.user << ": " << message.message_text << std::endl;
      break;
    case ServerMessage::SINGLE_MESSAGE:
      std::cout << '@' << message.user << "@: " << message.message_text << std::endl;
      break;
    case ServerMessage::ERROR_MESSAGE:
      std::cout << "ERROR: " << message.message_text << std::endl;
      break;
    case ServerMessage::DISCONNECT:
      std::cout << "You has been disconnected from server" << std::endl;
      break;
    case ServerMessage::SUCCESSFUL_CONNECT:
      std::cout << "Connected successful" << std::endl;
  }
  output_mutex.unlock();
}

void ConsoleChatController::get_user_input(std::string &result) {
  do {
    std::cin >> result;
  } while (result.length() > 0);
}

void ConsoleChatController::disconnect() {
  communicator.disconnect();
}

void ConsoleChatController::startWork() {
  login();
  std::function<void(ServerMessage &)> print = std::bind(static_print_message, this, std::placeholders::_1);
  communicator.start_receiving_messages(print);
  work_cycle();
  disconnect();
}

void ConsoleChatController::work_cycle() {
  while (true) {
    char command;
    std::cin >> command;
    switch (command) {
      case 'p':
        send_message();
        break;
      case 'b':
        send_broadcast();
      case 'q':
        return;
      default:
        std::cout << "Type p (private), b (broadcast) or q (quit)" << std::endl;
    }
  }
}

void static_print_message(ConsoleChatController *controller, ServerMessage &message) {
  controller->print_message(message);
}
