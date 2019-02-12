#include <utility>

//
// Created by iisuslik on 07.02.19.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <vector>
#include <map>
#include <string>
#include <signal.h>
#include <errno.h>

void printLog(const std::string &s);

class TestContainer {
public:
    struct Question {
        std::string text;
        std::vector<std::string> possibleAnswers = std::vector<std::string>();
        int correctAnswer;

        std::string toString() {
            std::string res = text + '&';
            for (int i = 0; i < possibleAnswers.size(); i++) {
                res += std::to_string(i) + ' ' + possibleAnswers[i] + '|';
            }
            return res;
        }
    };

    struct Test {
        std::string descriprion;
        std::vector<Question> questions = std::vector<Question>();
    };

    std::map<std::string, Test> tests = std::map<std::string, Test>();

    TestContainer();
};

class UserTests {
public:
    struct TestResult {
        std::vector<int> marks = std::vector<int>();

        int countFullMark() {
            int res = 0;
            for (int mark: marks) {
                res += mark;
            }
            return res;
        }

        std::string toString() {
            std::string res = "";
            for (int i = 0; i < marks.size(); i++) {
                res += std::to_string(i) + ' ' + std::to_string(marks[i]) + '|';
            }
            return res;
        }
    };

    UserTests(std::string login, TestContainer *container) : login(std::move(login)), container(container) {}

    UserTests() = default;

    TestResult getLastResult();

    TestContainer::Question *startTest(std::string testId);

    TestContainer::Question *answer(int answer);

    bool isCurrentTestFinished();

    std::string login = "";

    bool isAuthorized = false;

private:
    std::string currentTestId = "";
    TestContainer *container;
    TestResult curResult = TestResult();
    int curQuestion = -1;
};


class Server {
public:
    Server(uint16_t port, char split);

    ~Server();

    void start();

    void stop();

    std::vector<std::string> getUsers();

    bool kickClient(std::string login);

    void listenClient();

private:
    class ClientWorker {
    public:
        pthread_t tid;
        pthread_attr_t attr;
        Server *server;
        UserTests *tests;
        int number;
        pthread_mutex_t mutex;

        std::string login;

        void work();

        void startThread();

        void stop();


        ClientWorker(Server *server, int clientSockfd, int number) : clientSockfd(clientSockfd),
                                                                     server(server), number(number) {
            pthread_attr_init(&attr);
        }

        ~ClientWorker() {
            stop();
        }

    private:
        int clientSockfd;
        bool closed = false;

        bool handleRequest(std::string request);

        bool answerRequest(std::string commandCode, std::string body);

        void removeFromVector();

        void log(const std::string& s) {
            std::cout << "Worker # " << number << ": " << s << std::endl;
        }
    };

    static void *listenClientStatic(void *server) {
        ((Server *) server)->listenClient();
    }

    static void *workWithClientStatic(void *clientThreadInfo) {
        ((ClientWorker *) clientThreadInfo)->work();
    }

    int sockfd;
    struct sockaddr_in serv_addr;
    pthread_t serverThreadId;
    std::vector<ClientWorker *> workers;
    std::map<std::string, UserTests> users;
    TestContainer testContainer = TestContainer();
    int workerCounter = 0;
    char SPLIT;
    pthread_mutex_t workersMutex;
};


#endif //SERVER_SERVER_H
