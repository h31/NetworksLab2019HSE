#ifndef CALC_SERVER_H
#define CALC_SERVER_H
#include <netdb.h>
#include <netinet/in.h>
#include <vector>
#include <thread>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <mutex>
#include <unordered_set>
#include <atomic>


class Server {
public:
    Server(uint16_t portno);
    Server(const Server&) = delete;
    Server(Server&&) = default;
    void start();
    void cancel();
private:
    void handleClient(int newsockfd);

    sockaddr_in addr_;

    int sockfd_;
    std::vector<std::thread> threads_;
    std::atomic_int is_interrupted_ = {0};

    const static char SHORT_OPERATIONS = 0;
    const static char LONG_OPERATIONS = 1;

    const static char ADD = 1;
    const static char MULTIPLY = 2;
    const static char SUBTRACT = 3;
    const static char DIVIDE = 4;

    const static char FACTORIAL = 5;
    const static char SQRT = 6;

    const static char EXIT = 100;

    const static char OK = 0;
    const static char ERR_INCORRECT_ACTION = 1;
    const static char ERR_DIV_BY_ZERO = 2;
    const static char ERR_TOO_LONG = 3;
    const static char ERR_SQRT_NEG = 4;

    void startLongOperationsProcess(int newsockfd);
    void startShortOperationsProcess(int newsockfd);

    void writeArithmeticAnswer(int newsockfd, uint64_t id, double answer);
    void factorial(int newsockfd, std::mutex* mutex, uint64_t id, uint64_t n);
    void evalSqrt(int newsockfd, std::mutex* mutex, uint64_t id, double x);
};


#endif //CALC_SERVER_H
