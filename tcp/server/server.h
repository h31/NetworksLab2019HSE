#ifndef TEST_SYS_SERVER_H
#define TEST_SYS_SERVER_H
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


struct Test {
    std::string descrition;
    std::vector<std::pair<std::string, std::string>> questions_with_answers;
};

struct TestResult {
    uint32_t test_id;
    uint32_t number_of_correct_answers;
    uint32_t number_of_questions;
};


class Server {
public:
    Server(uint16_t portno, const std::vector<Test>& tests);
    void start();
    void cancel();
private:
    void handleClient(int newsokfd);
    std::pair<uint32_t, uint32_t> getLastTestResults(std::string user) const;
    std::string login(int newsokfd) const;
    std::string registerClient(int newsockfd);
    void sendTests(int newsockfd) const;
    void test(int newsockfd, const std::string& user);

    std::vector<std::thread> threadsForClients_;
    sockaddr_in addr_;
    const std::vector<Test> tests_;
    std::unordered_map<std::string, std::vector<TestResult>> results_;
    mutable std::mutex mutex_;
    mutable std::mutex mutex_for_users_;
    std::unordered_set<std::string> users_;
    int sockfd_;
    std::vector<std::thread> threads_;
    std::atomic_int is_interrupted_ = {0};

    const static char OK = 0;
    const static char INCORRECT_TEST_ID = 1;
    const static char LOGIN_NOT_FOUND = 2;
    const static char LOGIN_ALREADY_EXISTS = 3;
    const static char INCORRECT_ACTION = 4;

    const static char EXIT = 6;
    const static char WANT_TEST = 2;
    const static char END_OF_TEST = 5;
    const static char SEND_QUESTION = 3;
    const static char WANT_QUESTION = 4;
    const static char REGISTER_USER = 0;
    const static char LOG_IN = 1;
};


#endif //TEST_SYS_SERVER_H
