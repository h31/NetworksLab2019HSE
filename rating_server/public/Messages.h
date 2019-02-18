//
// Created by machine on 03-Feb-19.
//

#ifndef CHAT_CLIENT_MESSAGES_H
#define CHAT_CLIENT_MESSAGES_H

#include <cstdint>
#include <string>

struct ClientMessage {
  enum MessageType : uint8_t {
    CONNECT, NEW_RATING, DELETE_RATING, OPEN_RATING, CLOSE_RATING,
    ADD_CHOICE, RATING_LIST, SHOW_RATING, VOTE_RATING
  };

  ClientMessage(MessageType type) : type(type) {}

  ClientMessage(MessageType type, std::string *message_text) : type(type), message_text(message_text) {}

  ClientMessage(MessageType type, std::string *user, std::string *message_text)
      : type(type), user(user), message_text(message_text) {}

  MessageType type;
  std::string *user;
  std::string *message_text;
};

namespace ServerMessage {
  enum ServerMessageType : uint8_t {
    SUCCESS, ERROR, DISCONNECT, RATING_LIST, RATING_UNVOTED, RATING_VOTED
  };
}
#endif //CHAT_CLIENT_MESSAGES_H
