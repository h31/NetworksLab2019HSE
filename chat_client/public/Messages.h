//
// Created by machine on 03-Feb-19.
//

#ifndef CHAT_CLIENT_MESSAGES_H
#define CHAT_CLIENT_MESSAGES_H

#include <cstdint>
#include <string>

struct Message {
  enum MessageType : uint8_t {
    LOG_IN = 0x00, SINGLE_MESSAGE = 0x01, BROADCAST_MESSAGE = 0x02
  };

  Message(MessageType type) : type(type) {}

  Message(MessageType type, std::string *message_text) : type(type), message_text(message_text) {}

  Message(MessageType type, std::string *user, std::string *message_text)
      : type(type), user(user), message_text(message_text) {}

  MessageType type;
  std::string *user;
  std::string *message_text;
};

struct ServerMessage {
  enum ServerMessageType : uint8_t {
    SUCCESSFUL_CONNECT = 0xFF, SINGLE_MESSAGE = 0xFE, BROADCAST_MESSAGE = 0xFD, ERROR_MESSAGE = 0xFC, DISCONNECT = 0xFB
  };

  ~ServerMessage() {
    if (user != nullptr) {
      free(user);
      user = nullptr;
    }
    if (message_text != nullptr) {
      free(message_text);
      message_text = nullptr;
    }
  }

  ServerMessageType type;
  std::string *user = nullptr;
  std::string *message_text = nullptr;
};


#endif //CHAT_CLIENT_MESSAGES_H
