//
// Created by olga on 2/17/2019.
//

#ifndef NETWORKSLAB2019HSE_CLIENT_H
#define NETWORKSLAB2019HSE_CLIENT_H
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <unordered_map>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <atomic>

struct BinaryExpression {
    uint64_t id;
    char op;
    double left;
    double right;
};

struct UnaryExpression {
    uint64_t id;
    char op;
    double operand;
};

class Client {
public:
    Client(hostent *server, uint16_t portno);
    void start();
    Client(const Client&) = delete;
    Client(Client&&) = default;
private:
    void processShortExpression(const BinaryExpression& expression);
    void processLongExpression(const UnaryExpression& expression);
    void workWithLong();
    void flush();
    int long_sockfd_;
    int short_sockfd_;
    std::unordered_map<uint64_t, double> long_op_res_;
    std::unordered_map<uint64_t, UnaryExpression> long_op_descr_;
    std::atomic_bool is_interrupted_ = {0};
    mutable std::mutex long_expression_mutex_;
    const static std::unordered_map<char, char> OP_CODES;
    const static char OK = 0;
    const static char ERR_INCORRECT_ACTION = 1;
    const static char ERR_DIV_BY_ZERO = 2;
    const static char ERR_TOO_LONG = 3;
    const static char ERR_SQRT_NEG = 4;
};

#endif //NETWORKSLAB2019HSE_CLIENT_H
