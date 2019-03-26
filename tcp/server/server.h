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
#include <optional>


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
    Server(uint16_t portno, const std::vector<Test> &tests);

    void start();

private:
    enum State {
        REGISTER_USER = 0,
        LOG_IN = 1,
        WANT_TEST = 2,
        SEND_QUESTION = 3,
        WANT_QUESTION = 4,
        END_OF_TEST = 5,
        EXIT = 6
    };

    enum ExitCode {
        OK = 0,
        INCORRECT_TEST_ID = 1,
        LOGIN_NOT_FOUND = 2,
        LOGIN_ALREADY_EXISTS = 3,
        INCORRECT_ACTION = 4
    };

    void handleClient(int newsokfd);

    std::pair<uint32_t, uint32_t> getLastTestResults(std::string user) const;

    std::optional<std::string> login(int newsokfd) const;

    std::optional<std::string> registerClient(int newsockfd);

    void sendTests(int newsockfd) const;

    void test(int newsockfd, const std::string &user);

    void handleIncorrectUserInput(ExitCode code_to_send, int newsockfd) const;

    std::pair<uint32_t, uint32_t>
    processAnswers(uint32_t test_id, const std::vector<std::string> &answers,
                   const std::string &user);

    const std::vector<Test> tests_;
    std::unordered_map<std::string, std::vector<TestResult>> results_;
    mutable std::mutex mutex_;
    mutable std::mutex mutex_for_users_;
    std::unordered_set<std::string> users_;
    int sockfd_;
    std::vector<std::thread> threads_;
    std::atomic_int is_interrupted_ = {0};
};


#endif //TEST_SYS_SERVER_H
