#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

#include "server.h"


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


std::string readStr(int newsockfd) {
    uint32_t str_size = 0;
    safeRead(newsockfd, (char *) &str_size, sizeof(str_size));
    str_size = le32toh(str_size);
    char buffer[256];
    uint32_t bytes_read = 0;
    std::string res;
    while (bytes_read < str_size) {
        bzero(buffer, 256);
        int num = safeRead(newsockfd, buffer, std::min(str_size - bytes_read, (uint32_t) 256));
        res += std::string(buffer, (size_t) num);
        bytes_read += num;
    }
    std::cout << res << std::endl;
    return res;
}


void sendLastResults(int newsockfd, char code, const std::pair<uint32_t, uint32_t> &results) {
    safeWrite(newsockfd, &code, 1);
    uint32_t correct = htole32(results.first);
    safeWrite(newsockfd, &correct, sizeof(results.first));
    uint32_t all = htole32(results.second);
    safeWrite(newsockfd, &all, sizeof(results.second));
}


Server::Server(uint16_t portno, const std::vector<Test> &tests) : tests_(tests) {
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd_ < 0) {
        std::cerr << "ERROR opening socket\n";
        throw SocketException();
    }

    sockaddr_in serv_addr;
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
    sockaddr_in cli_addr;
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


void Server::cancel() {
    is_interrupted_ = 1;
}



void Server::handleClient(int newsockfd) {
    char code = 0;
    try {
        safeRead(newsockfd, &code, 1);
        std::string user;
        switch (code) {
            case REGISTER_USER:
                user = registerClient(newsockfd);
                break;
            case LOG_IN:
                user = login(newsockfd);
                break;
            default:
                char err_code = INCORRECT_ACTION;
                safeWrite(newsockfd, &err_code, 1);
                close(newsockfd);
                return;
        }
        sendTests(newsockfd);
        test(newsockfd, user);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        close(newsockfd);
        return;
    }
}


std::pair<uint32_t, uint32_t> Server::getLastTestResults(std::string user) const {
    mutex_.lock();
    auto it = results_.find(user);
    if (it != results_.end() && !it->second.empty()) {
        TestResult res = it->second.back();
        mutex_.unlock();
        return {res.number_of_correct_answers, res.number_of_questions};
    }
    mutex_.unlock();
    return {0, 0};
}


std::string Server::login(int newsockfd) const {
    std::string login = readStr(newsockfd);
    mutex_for_users_.lock();
    if (users_.find(login) == users_.end()) {
        char code = LOGIN_NOT_FOUND;
        safeWrite(newsockfd, &code, sizeof(code));
        close(newsockfd);
        mutex_for_users_.unlock();
        return "";
    }
    mutex_for_users_.unlock();
    std::pair<uint32_t, uint32_t> last_results = getLastTestResults(login);
    sendLastResults(newsockfd, OK, last_results);
    return login;
}


std::string Server::registerClient(int newsockfd) {
    std::string login = readStr(newsockfd);
    mutex_for_users_.lock();
    if (users_.find(login) != users_.end()) {
        char code = LOGIN_ALREADY_EXISTS;
        write(newsockfd, &code, sizeof(code));
        close(newsockfd);
        mutex_for_users_.unlock();
        return "";
    }
    users_.insert(login);
    mutex_for_users_.unlock();
    sendLastResults(newsockfd, 0, std::make_pair(0, 0));
    return login;
}


void Server::sendTests(int newsockfd) const {
    uint32_t number_of_tests = (uint32_t) htole32(tests_.size());
    safeWrite(newsockfd, &number_of_tests, sizeof(number_of_tests));
    for (size_t i = 0; i < tests_.size(); i++) {
        uint32_t id = (uint32_t) htole32(i);
        uint32_t num_of_bytes = (uint32_t) strlen(tests_[i].descrition.c_str());
        uint32_t test_size = htole32(num_of_bytes);
        safeWrite(newsockfd, &id, sizeof(id));
        safeWrite(newsockfd, &test_size, sizeof(test_size));
        safeWrite(newsockfd, tests_[i].descrition.c_str(), num_of_bytes);
    }
}


void Server::test(int newsockfd, const std::string &user) {
    char client_code = 0;
    safeRead(newsockfd, &client_code, sizeof(client_code));
    while (client_code != EXIT) {
        char op_code = 0;
        if (client_code != WANT_TEST) {
            op_code = INCORRECT_ACTION;
            safeWrite(newsockfd, &op_code, 1);
            close(newsockfd);
            return;
        }
        uint32_t test_id = 0;
        safeRead(newsockfd, &test_id, sizeof(test_id));
        test_id = le32toh(test_id);
        if (test_id >= tests_.size()) {
            op_code = INCORRECT_TEST_ID;
            safeWrite(newsockfd, &op_code, 1);
            close(newsockfd);
            return;
        }
        safeWrite(newsockfd, &op_code, 1);
        uint32_t number_of_correct_answers = 0;
        const auto &q_and_a = tests_[test_id].questions_with_answers;
        for (size_t i = 0; i < q_and_a.size(); i++) {
            char code = SEND_QUESTION;
            safeWrite(newsockfd, &code, 1);
            uint32_t id = (uint32_t) htole32(i);
            safeWrite(newsockfd, &id, sizeof(id));
            uint32_t num_of_bytes = (uint32_t) strlen(q_and_a[i].first.c_str());
            uint32_t question_size = (uint32_t) htole32(num_of_bytes);
            safeWrite(newsockfd, &question_size, sizeof(question_size));
            safeWrite(newsockfd, q_and_a[i].first.c_str(), num_of_bytes);

            safeRead(newsockfd, &client_code, sizeof(client_code));
            if (client_code == EXIT) {
                close(newsockfd);
                return;
            }
            if (client_code != WANT_QUESTION) {
                op_code = INCORRECT_TEST_ID;
                safeWrite(newsockfd, &op_code, 1);
                close(newsockfd);
                return;
            }
            std::string answer = readStr(newsockfd);
            if (answer == q_and_a[i].second) {
                number_of_correct_answers++;
            }
        }
        char code = END_OF_TEST;
        sendLastResults(newsockfd, code, {number_of_correct_answers, (uint32_t) q_and_a.size()});
        mutex_.lock();
        TestResult test_result = {test_id, number_of_correct_answers, (uint32_t) q_and_a.size()};
        results_[user].push_back(test_result);
        mutex_.unlock();
        safeRead(newsockfd, &client_code, sizeof(client_code));
    }
    close(newsockfd);
}







