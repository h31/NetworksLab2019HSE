#ifndef TCP_MESSAGE_H
#define TCP_MESSAGE_H

#include <string>

enum MessageType
{
    // TODO:
};

class Message
{
  public:
    Message();

    Message(char* data, MessageType& type);

    bool Serialize(int sockfd) const;

    static Message Deserialize(int sockfd);

    MessageType GetMessageType() const;

    char* GetBodyData() const;

  private:
    MessageType type_;
    
    char* body_;
};

#endif // TCP_MESSAGE_H