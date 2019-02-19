#ifndef CHAT_CLIENT_MESSAGES_H
#define CHAT_CLIENT_MESSAGES_H

namespace ServerMessage {
  enum MessageType : uint8_t {
    SUCCESS = 0, ERROR = 1, DISCONNECT = 2, RATING_LIST = 3, RATING_STATS = 4
  };
}
#endif //CHAT_CLIENT_MESSAGES_H
