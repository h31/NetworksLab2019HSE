#include <iostream>
#include <thread>

#include "client.h"

class SocketException : public std::exception {
    virtual const char *what() const throw() {
        return "Problems with socket";
    }
};


int safeRead(int newsockfd, void *buf, size_t num) {
    int n = (int) read(newsockfd, buf, num);
    if (n < 0) {
        throw SocketException();
    }
    return n;
}


void safeWrite(int newsockfd, const void *buf, size_t num) {
    int n = (int) write(newsockfd, buf, num);
    if (n < 0) {
        throw SocketException();
    }
}

std::string describe(const UnaryExpression &expression) {
    if (expression.op == '!') {
        return std::to_string(expression.operand) + "!";
    }
    return "sqrt " + std::to_string(expression.operand);
}


Client::Client(hostent *server, uint16_t portno) {
    long_sockfd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (long_sockfd_ < 0) {
        std::cerr << "ERROR on opening socket\n";
        throw SocketException();
    }

    short_sockfd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (short_sockfd_ < 0) {
        std::cerr << "ERROR on opening socket\n";
        throw SocketException();
    }

    sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(portno);

    /* Now connect to the server */
    if (connect(long_sockfd_, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "ERROR connecting\n";
        throw SocketException();
    }

    if (connect(short_sockfd_, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "ERROR connecting\n";
        throw SocketException();
    }
}


void Client::start() {
    char l_code = 1;
    char s_code = 0;
    safeWrite(long_sockfd_, &l_code, sizeof(l_code));
    safeWrite(short_sockfd_, &s_code, sizeof(s_code));
    std::thread long_operations(&Client::workWithLong, this);
    //std::thread listen_short(&)
    std::string cmd;
    uint64_t id = 0;
    try {
        while (!is_interrupted_) {
            id++;
            std::cin >> cmd;
            if (cmd == "exit") {
                char exit_code = OP_CODES['e'];
                safeWrite(long_sockfd_, &exit_code, sizeof(exit_code));
                safeWrite(short_sockfd_, &exit_code, sizeof(exit_code));
                is_interrupted_ = true;
                break;
            }
            if (cmd == "sqrt") {
                double num;
                std::cin >> num;
                processLongExpression({id, 's', num});
                continue;
            }
            char op;
            std::cin >> op;
            if (op == '!') {
                processLongExpression({id, '!', std::stoi(cmd)});
                continue;
            }
            if (op == '*' || op == '-' || op == '+' || op == '/') {
                double num;
                std::cin >> num;
                processShortExpression({id, op, std::stod(cmd), num});
            } else {
                std::cerr << "invalid expression\n";
            }
        }
    } catch (SocketException& e) {
        std::cerr << "Error on socket\n";
    }
    long_operations.join();
    flush();
}


void Client::processLongExpression(const UnaryExpression &expression) {
    long_op_descr_[expression.id] = expression;
    char op_code = OP_CODES[expression.op];
    safeWrite(long_sockfd_, &op_code, sizeof(op_code));
    uint64_t id = htole(expression.id);
    safeWrite(long_sockfd_, &id, sizeof(id));
    double operand = htole(expression.arg);
    safeWrite(long_sockfd_, &operand, sizeof(operand));
}


void Client::processShortExpression(const BinaryExpression &expression) {
    char op_code = OP_CODES[expression.op];
    safeWrite(short_sockfd_, &op_code, sizeof(op_code));
    uint64_t id = expression.id;
    safeWrite(short_sockfd_, &id, sizeof(id));
    double left = expression.left;
    safeWrite(short_sockfd_, &left, sizeof(left));
    double right = expression.right;
    safeWrite(short_sockfd_, &right, sizeof(right));
    char res_code;
    safeRead(short_sockfd_, &res_code, sizeof(res_code));
    if (res_code != OK) {
        std::cerr << "Error on server\n";
        if (res_code != ERR_INCORRECT_ACTION) {
            uint64_t new_id;
            safeRead(short_sockfd_, &new_id, sizeof(new_id));
        }
        return;
    }
    uint64_t new_id;
    safeRead(short_sockfd_, &new_id, sizeof(new_id));
    double res;
    safeRead(short_sockfd_, &res, sizeof(res));
    std::cout << res << std::endl;
}


void Client::workWithLong() {
    try {
        while (!is_interrupted_) {
            char op;
            int n = safeRead(long_sockfd_, &op, sizeof(op));
            if (n > 0) {
                if (op != 0) {
                    std::cerr << "Error on server\n";
                    continue;
                }
                uint64_t id;
                safeRead(long_sockfd_, &id, sizeof(id));
                double res;
                safeRead(long_sockfd_, &res, sizeof(res));
                long_op_res_[id] = res;
            }
        }
    } catch (SocketException& e) {
        std::cerr << "Error on socket\n";
        is_interrupted_ =  true;
    }
}

void Client::flush() {
    for (const auto &it : long_op_res_) {
        std::cout << describe(long_op_descr_[it.first]) << ": " << it.second << std::endl;
    }
}




