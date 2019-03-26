#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

#include "server.h"


class SocketException : public std::exception {
    const char *what() const noexcept override {
        return "Problems with socket";
    }
};


void safeRead(int newsockfd, void *buf, size_t num) {
    int n = (int) read(newsockfd, buf, num);
    if (n < 0) {
        throw SocketException();
    }
}


void safeWrite(int newsockfd, const void *buf, size_t num) {
    int n = (int) write(newsockfd, buf, num);
    if (n < 0) {
        throw SocketException();
    }
}


std::string readStr(int newsockfd) {
    uint32_t str_size = 0;
    safeRead(newsockfd, (char *) &str_size, sizeof(str_size));
    str_size = le32toh(str_size);
    char *buffer = new char[str_size + 1];
    try {
        safeRead(newsockfd, buffer, str_size);
    } catch (SocketException &e) {
        delete[] buffer;
        throw e;
    }
    buffer[str_size] = '\0';
    std::string res(buffer);
    delete[] buffer;
    return res;
}


void sendLastResults(int newsockfd, char code,
                     const std::pair<uint32_t, uint32_t> &results) {
    safeWrite(newsockfd, &code, 1);
    uint32_t correct = htole32(results.first);
    safeWrite(newsockfd, &correct, sizeof(results.first));
    uint32_t all = htole32(results.second);
    safeWrite(newsockfd, &all, sizeof(results.second));
}


Server::Server(uint16_t portno, const std::vector<Test> &tests) : tests_(
        tests) {
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd_ < 0) {
        std::cerr << "ERROR opening socket\n";
        throw SocketException();
    }

    sockaddr_in serv_addr{};
    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd_, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "ERROR ON BINDING\n";
        throw SocketException();
    }
}


void Server::start() {
    listen(sockfd_, 500);
    sockaddr_in cli_addr{};
    unsigned int clilen = sizeof(cli_addr);

    /* Accept actual connection from the client */
    while (!is_interrupted_) {
        int newsockfd = accept(sockfd_, (sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            std::cerr << "Cannot accept client\n";
            is_interrupted_ = 1;
        } else {
            std::thread t(&Server::handleClient, this, newsockfd);
            threads_.push_back(std::move(t));
        }
    }
    for (auto &thread : threads_) {
        thread.join();
    }
}


void Server::handleIncorrectUserInput(Server::ExitCode code_to_send,
                                       int newsockfd) const {
    char code = code_to_send;
    safeWrite(newsockfd, &code, sizeof(code));
    close(newsockfd);
}


void Server::handleClient(int newsockfd) {
    char code = 0;
    try {
        safeRead(newsockfd, &code, 1);
        std::optional<std::string> userOr;
        switch (code) {
            case State::REGISTER_USER:
                userOr = registerClient(newsockfd);
                break;
            case State::LOG_IN:
                userOr = login(newsockfd);
                break;
            default:
                handleIncorrectUserInput(ExitCode::INCORRECT_ACTION,
                                          newsockfd);
                return;
        }
        if (!userOr.has_value()) {
            return;
        }
        auto last_results = getLastTestResults(userOr.value());
        sendLastResults(newsockfd, ExitCode::OK, last_results);
        sendTests(newsockfd);
        test(newsockfd, userOr.value());
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        close(newsockfd);
        return;
    }
}


std::pair<uint32_t, uint32_t>
Server::getLastTestResults(std::string user) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = results_.find(user);
    if (it != results_.end() && !it->second.empty()) {
        TestResult res = it->second.back();
        return {res.number_of_correct_answers, res.number_of_questions};
    }
    return {0, 0};
}

std::optional<std::string> Server::login(int newsockfd) const {
    std::string login = readStr(newsockfd);
    bool login_exists;
    {
        std::lock_guard<std::mutex> lock(mutex_for_users_);
        login_exists = users_.find(login) == users_.end();
    }
    if (!login_exists) {
        handleIncorrectUserInput(ExitCode::LOGIN_NOT_FOUND, newsockfd);
        return {};
    }
    return login;
}


std::optional<std::string> Server::registerClient(int newsockfd) {
    std::string login = readStr(newsockfd);
    std::lock_guard<std::mutex> lock(mutex_for_users_);
    if (users_.find(login) != users_.end()) {
        handleIncorrectUserInput(ExitCode::LOGIN_ALREADY_EXISTS, newsockfd);
        return {};
    }
    users_.insert(login);
    return login;
}


void Server::sendTests(int newsockfd) const {
    auto number_of_tests = (uint32_t) htole32((uint32_t )tests_.size());
    safeWrite(newsockfd, &number_of_tests, sizeof(number_of_tests));
    for (uint32_t i = 0; i < tests_.size(); i++) {
        auto id = (uint32_t) htole32(i);
        auto num_of_bytes = (uint32_t) tests_[i].descrition.length();
        uint32_t test_size = htole32(num_of_bytes);
        safeWrite(newsockfd, &id, sizeof(id));
        safeWrite(newsockfd, &test_size, sizeof(test_size));
        safeWrite(newsockfd, tests_[i].descrition.c_str(), num_of_bytes);
    }
}


void Server::test(int newsockfd, const std::string &user) {
    char client_code = 0;
    safeRead(newsockfd, &client_code, sizeof(client_code));
    while (client_code != State::EXIT) {
        if (client_code != State::WANT_TEST) {
            handleIncorrectUserInput(ExitCode::INCORRECT_ACTION, newsockfd);
            return;
        }
        uint32_t test_id = 0;
        safeRead(newsockfd, &test_id, sizeof(test_id));
        test_id = le32toh(test_id);
        if (test_id >= tests_.size()) {
            handleIncorrectUserInput(ExitCode::INCORRECT_TEST_ID, newsockfd);
            return;
        }
        char op_code = ExitCode::OK;
        safeWrite(newsockfd, &op_code, 1);
        const auto &q_and_a = tests_[test_id].questions_with_answers;
        std::vector<std::string> answers;
        for (uint32_t i = 0; i < q_and_a.size(); i++) {
            char code = State::SEND_QUESTION;
            safeWrite(newsockfd, &code, 1);
            auto id = (uint32_t) htole32(i);
            safeWrite(newsockfd, &id, sizeof(id));
            auto num_of_bytes = (uint32_t) q_and_a[i].first.length();
            auto question_size = (uint32_t) htole32(num_of_bytes);
            safeWrite(newsockfd, &question_size, sizeof(question_size));
            safeWrite(newsockfd, q_and_a[i].first.c_str(), num_of_bytes);
            safeRead(newsockfd, &client_code, sizeof(client_code));
            if (client_code == State::EXIT) {
                close(newsockfd);
                return;
            }
            if (client_code != State::WANT_QUESTION) {
                handleIncorrectUserInput(ExitCode::INCORRECT_ACTION,
                                          newsockfd);
                return;
            }
            std::string answer = readStr(newsockfd);
            answers.push_back(std::move(answer));
        }
        char code = State::END_OF_TEST;
        sendLastResults(newsockfd, code,
                        processAnswers(test_id, answers, user));
        safeRead(newsockfd, &client_code, sizeof(client_code));
    }
    close(newsockfd);
}


std::pair<uint32_t, uint32_t>
Server::processAnswers(uint32_t test_id,
                       const std::vector<std::string> &answers,
                       const std::string &user) {
    uint32_t number_of_correct_answers = 0;
    for (int i = 0; i < answers.size(); i++) {
        if (answers[i] == tests_[test_id].questions_with_answers[i].second) {
            number_of_correct_answers++;
        }
    }
    std::lock_guard<std::mutex> lock(mutex_);
    TestResult test_result = {test_id, number_of_correct_answers,
                              (uint32_t) answers.size()};
    results_[user].push_back(test_result);
    return {number_of_correct_answers, (uint32_t) answers.size()};
}







