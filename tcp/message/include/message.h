#ifndef TCP_MESSAGE_H
#define TCP_MESSAGE_H

#include <string>

enum MessageType
{
    CALCULATION,
    RESULT
};

class Calculation
{
  public:
    Calculation(char operation, int arg_left, int arg_right):
        operation(operation), arg_left(arg_left), arg_right(arg_right), result(0){};

    Calculation(char operation, int arg_left, int arg_right, double result):
            operation(operation), arg_left(arg_left), arg_right(arg_right), result(result){};

    char* Serialize() const;

    static Calculation Deserialize(char* bytes);
    int GetArgLeft();
    int GetArgRight();
    char GetOperation();
    double GetResult();

  private:
    char operation;
    int arg_left, arg_right;
    double result;
};
#endif // TCP_MESSAGE_H