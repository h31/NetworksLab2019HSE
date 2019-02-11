#ifndef TCP_MESSAGE_H
#define TCP_MESSAGE_H

#include <string>

enum MessageType
{
    CALCULATION,
    RESULT,
    RESPONSE
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

class Calculation
{
  public:
    Calculation(char operation, int arg_left, int arg_right):
        operation(operation), arg_left(arg_left), arg_right(arg_right){};

    char* Serialize() const;

    static Calculation Deserialize(char* bytes);
    int GetArgLeft();
    int GetArgRight();
    char GetOperation();
    
  private:
    char operation;

    int arg_left, arg_right;

};

class Response
{
  public:
    Response(int id): id(id){};
    char* Serialize() const;
    static Response Deserialize(char* bytes);
    int GetId();

  private:
    int id;
};

class Result
{
  public:
    Result(double value): value(value){};
    char* Serialize() const;
    static Result Deserialize(char* bytes);
    double GetValue();

  private:
    double value;
};
#endif // TCP_MESSAGE_H