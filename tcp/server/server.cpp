#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <math.h>

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


Server::Server(uint16_t portno) {
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

void Server::writeArithmeticAnswer(int newsockfd, uint64_t id, double answer) {
    char code = Server::OK;
    safeWrite(newsockfd, &code, 1);
    safeWrite(newsockfd, &id, sizeof(id));
    safeWrite(newsockfd, &answer, sizeof(answer));
}

void Server::startShortOperationsProcess(int newsockfd) {
    char code;
    safeRead(newsockfd, &code, 1);
    uint64_t id;
    double left, right, answer;
    while (code != Server::EXIT) {
        switch (code) {
            case Server::ADD: {
                safeRead(newsockfd, &id, sizeof(id));
                safeRead(newsockfd, &left, sizeof(left));
                safeRead(newsockfd, &right, sizeof(right));
                std::cerr << newsockfd << " Got arithmetic operation add: " << left << " + " << right << "\n";
                answer = left + right;
                writeArithmeticAnswer(newsockfd, id, answer);
                break;
            }
            case Server::MULTIPLY: {
                safeRead(newsockfd, &id, sizeof(id));
                safeRead(newsockfd, &left, sizeof(left));
                safeRead(newsockfd, &right, sizeof(right));
                std::cerr << newsockfd << " Got arithmetic operation multiply: " << left << " * " << right << "\n";
                answer = left * right;
                writeArithmeticAnswer(newsockfd, id, answer);
                break;
            }
            case Server::SUBTRACT: {
                safeRead(newsockfd, &id, sizeof(id));
                safeRead(newsockfd, &left, sizeof(left));
                safeRead(newsockfd, &right, sizeof(right));
                std::cerr << newsockfd << " Got arithmetic operation subtract: " << left << " - " << right << "\n";
                answer = left - right;
                writeArithmeticAnswer(newsockfd, id, answer);
                break;
            }
            case Server::DIVIDE: {
                safeRead(newsockfd, &id, sizeof(id));
                safeRead(newsockfd, &left, sizeof(left));
                safeRead(newsockfd, &right, sizeof(right));
                if (right == 0) {
                    char err_code = Server::ERR_DIV_BY_ZERO;
                    safeWrite(newsockfd, &err_code, 1);
                    safeWrite(newsockfd, &id, sizeof(id));
                    break;
                }
                std::cerr << newsockfd << " Got arithmetic operation divide: " << left << " / " << right << "\n";
                answer = left / right;
                writeArithmeticAnswer(newsockfd, id, answer);
                break;
            }
            default: {
                char err_code = Server::ERR_INCORRECT_ACTION;
                safeWrite(newsockfd, &err_code, 1);
                close(newsockfd);
                return;
            }
        }
        safeRead(newsockfd, &code, 1);
    }
    std::cerr << "Closed " << newsockfd << "\n";
    close(newsockfd);
}

void Server::factorial(int newsockfd, std::mutex* mutex, uint64_t id, uint64_t n) {
    double result = 1;
    if (n > 1000000) {
        mutex->lock();
        char err_code = Server::ERR_TOO_LONG;
        safeWrite(newsockfd, &err_code, 1);
        safeWrite(newsockfd, &id, sizeof(id));
        mutex->unlock();
        return;
    }
    std::cerr << newsockfd <<" Got operation factorial: " << n << "!\n";
    for (uint64_t i = 2; i <= n; i++) {
        result *= i;
    }
    mutex->lock();
    char code = Server::OK;
    safeWrite(newsockfd, &code, 1);
    safeWrite(newsockfd, &id, sizeof(id));
    safeWrite(newsockfd, &result, sizeof(result));
    mutex->unlock();
}

void Server::evalSqrt(int newsockfd, std::mutex* mutex, uint64_t id, double x) {
    if (x < 0) {
        mutex->lock();
        char err_code = Server::ERR_SQRT_NEG;
        safeWrite(newsockfd, &err_code, 1);
        safeWrite(newsockfd, &id, sizeof(id));
        mutex->unlock();
        return;
    }
    std::cerr << newsockfd << " Got operation sqrt: sqrt " << x << "\n";
    double result = sqrt(x);
    mutex->lock();
    char code = Server::OK;
    safeWrite(newsockfd, &code, 1);
    safeWrite(newsockfd, &id, sizeof(id));
    safeWrite(newsockfd, &result, sizeof(result));
    mutex->unlock();
}

void Server::startLongOperationsProcess(int newsockfd) {
    std::mutex personal_mutex;
    std::vector<std::thread> eval_threads;
    char code;
    safeRead(newsockfd, &code, 1);
    uint64_t id;
    while (code != Server::EXIT) {
        switch (code) {
            case Server::FACTORIAL: {
                safeRead(newsockfd, &id, sizeof(id));
                uint64_t n;
                safeRead(newsockfd, &n, sizeof(n));
                std::thread t(&Server::factorial, this, newsockfd, &personal_mutex, id, n);
                eval_threads.push_back(std::move(t));
                break;
            }
            case Server::SQRT: {
                safeRead(newsockfd, &id, sizeof(id));
                double x;
                safeRead(newsockfd, &x, sizeof(x));
                std::thread t(&Server::evalSqrt, this, newsockfd, &personal_mutex, id, x);
                eval_threads.push_back(std::move(t));
                break;
            }
            default: {
                char err_code = Server::ERR_INCORRECT_ACTION;
                safeWrite(newsockfd, &err_code, 1);
                close(newsockfd);
                return;
            }
        }
        safeRead(newsockfd, &code, 1);
    }
    for (auto &thread : eval_threads) {
        thread.join();
    }
    std::cerr << "Closed " << newsockfd << "\n";
    close(newsockfd);
}


void Server::handleClient(int newsockfd) {
    char code = 0;
    try {
        safeRead(newsockfd, &code, 1);
        if (code != 0) {
            char err_code = Server::ERR_INCORRECT_ACTION;
            safeWrite(newsockfd, &err_code, 1);
            close(newsockfd);
            return;
        }
        safeRead(newsockfd, &code, 1);
        switch (code) {
            case Server::SHORT_OPERATIONS: {
                std::cerr << newsockfd << " short operations started\n";
                startShortOperationsProcess(newsockfd);
                break;
            }
            case Server::LONG_OPERATIONS: {
                std::cerr << newsockfd << " long operations started\n";
                startLongOperationsProcess(newsockfd);
                break;
            }
            default: {
                char err_code = Server::ERR_INCORRECT_ACTION;
                safeWrite(newsockfd, &err_code, 1);
                close(newsockfd);
                return;
            }
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        close(newsockfd);
        return;
    }
}





