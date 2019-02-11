//
// Created by iisuslik on 07.02.19.
//

#include "../include/server.h"

void printLog(const std::string &s) {
    std::cout << s << std::endl;
}

Server::Server(uint16_t port) {
    printLog("Starting server on port " + std::to_string(port));
    workers = std::vector<ClientWorker>();
    users = std::map<std::string, UserTests>();
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }
    printLog("Socket binded");
}

void Server::listenClient() {
    struct sockaddr_in cli_addr;
    listen(sockfd, 5);
    unsigned int clilen = sizeof(cli_addr);
    while (true) {
        int clientSockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (clientSockfd < 0) {
            perror("ERROR on accept");
            exit(1);
        }
        printLog("Got a client!!!");
        workers.emplace_back(this, clientSockfd);
        auto worker = &workers[workers.size() - 1];
        worker->startThread();
    }
}


void Server::start() {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    int createRes = pthread_create(&serverThreadId, &attr, listenClientStatic, this);
    if (createRes < 0) {
        perror("ERROR on creating server thread");
        exit(1);
    }
    printLog("Listener-thread started with tid " + std::to_string(serverThreadId));\

}

void Server::stop() {
    for (auto worker: workers) {
        worker.stop();
    }
    close(sockfd);
}


bool Server::kickClient(std::string login) {

}

std::vector<std::string> Server::getUsers() {
    std::vector<std::string> res = std::vector<std::string>();
    for (auto worker: workers) {
        if (worker.login != "") {
            res.push_back(worker.login);
        }
    }
    return res;
}

Server::~Server() {
    std::cout << "Closing server socket" << std::endl;
    close(sockfd);
}


void Server::ClientWorker::startThread() {
    int createRes = pthread_create(&tid, &attr, workWithClientStatic, this);
    if (createRes < 0) {
        perror("ERROR on creating server thread");
        exit(1);
    }
}


void Server::ClientWorker::removeFromVector() {
    for (auto ptr = server->workers.begin(); ptr < server->workers.end(); ptr++) {
        if (ptr.base()->tid == tid) {
            server->workers.erase(ptr);
            return;
        }
    }
}

void Server::ClientWorker::work() {
    printLog("Client worker started with tid " + std::to_string(tid));
    char buffer[256];
    while (true) {
        std::string command = "";
        bzero(buffer, 256);
        size_t n = static_cast<size_t>(read(clientSockfd, buffer, 255));
        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }
        if (n == 0) {
            removeFromVector();
            return;
        }
        for (size_t i = 0; i < n; i++) {
            if (buffer[i] == SPLIT) {
                bool stopWorking = handleRequest(command);
                if (stopWorking) {
                    removeFromVector();
                    return;
                }
                command = "";
            } else {
                command += buffer[i];
            }
        }
    }
}


bool Server::ClientWorker::handleRequest(std::string request) {
    if (request.length() < 5 || request[4] != ' ') {
        printLog("BAD request: " + request);
        return false;
    }
    printLog("CORRECT request: " + request);
    std::string commandCode = request.substr(0, 4);
    if (commandCode == "GBYE") {
        return true;
    }
    std::string body = request.substr(5);
    answerRequest(commandCode, body);
    return false;
}

void Server::ClientWorker::answerRequest(std::string commandCode, std::string body) {
    std::string responseStatus;
    std::string responseBody = "";
    if (commandCode == "REGI") {
        if (server->users.count(body)) {
            responseStatus = "UE";
        } else {
            responseStatus = "OK";
            server->users.insert(std::make_pair(body, UserTests(body, &server->testContainer)));
            tests = &server->users[body];
            tests->isAuthorized = true; // usefull for testing
            login = body;
        }
    } else if (commandCode == "AUTH") {
        if (server->users.count(body) == 1) {
            if (server->users[body].isAuthorized) {
                // someone has authorized with this login
                // TODO new error code
                responseStatus = "UN";
            } else {
                responseStatus = "OK";
                server->users[body].isAuthorized = true;
                login = body;
            }
        } else {
            responseStatus = "UN";
        }
    } else {
        if (login.empty()) {
            responseStatus = "NA";
        } else if (commandCode == "LIST") {
            responseStatus = "OK";
            for (auto &test: server->testContainer.tests) {
                responseBody += test.first + '&' + test.second.descriprion + '|';
            }
        } else if (commandCode == "TEST") {
            TestContainer::Question *firstQuestion = tests->startTest(body);
            if (firstQuestion) {
                responseStatus = "OK";
                responseBody = firstQuestion->toString();
            } else {
                responseStatus = "TN";
            }
        } else if (commandCode == "ANSW") {
            TestContainer::Question *nextQuestion = tests->answer(std::stoi(body));
            if (nextQuestion) {
                responseStatus = "OK";
                responseBody = nextQuestion->toString();
            } else {
                if (tests->isCurrentTestFinished()) {
                    responseStatus = "TN";
                    responseBody = tests->getLastResult().toString();
                } else {
                    responseStatus = "BR";
                }
            }
        } else if (commandCode == "LAST") {
            responseStatus = "OK";
            responseBody = tests->getLastResult().toString();

        } else {
            // unknown commandCode
            responseStatus = "BR";
        }
    }
    std::string result = commandCode + ' ' + responseStatus + ' ' + responseBody + SPLIT;
    int n = write(clientSockfd, result.c_str(), result.size());
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
}

void Server::ClientWorker::stop() {
    if (tests) {
        tests->isAuthorized = false;
    }
    close(clientSockfd);
}


UserTests::TestResult UserTests::getLastResult() {
    return curResult;
}

TestContainer::Question *UserTests::startTest(std::string testId) {
    if (!container->tests.count(testId)) {
        printLog("wrong id");
        return NULL;
    }
    curQuestion = 0;
    currentTestId = testId;
    curResult.marks.clear();
    return &container->tests[testId].questions[curQuestion];
}

TestContainer::Question *UserTests::answer(int answer) {
    if (currentTestId.empty() || curQuestion == -1 || isCurrentTestFinished()) {
        return NULL;
    }
    curResult.marks.push_back(container->tests[currentTestId].questions[curQuestion].correctAnswer == answer ? 1 : 0);
    curQuestion++;
    if (isCurrentTestFinished()) {
        curQuestion = -1;
        return NULL;
    } else {
       return &container->tests[currentTestId].questions[curQuestion];
    }
}

bool UserTests::isCurrentTestFinished() {
    return curQuestion == -1 || curQuestion == container->tests[currentTestId].questions.size();
}

TestContainer::TestContainer() {
    tests.insert(std::make_pair("1", Test()));
    tests["1"].descriprion = "How well do you know David Bowie";
    auto question1 = Question();
    question1.text = "Is he gay?";
    question1.possibleAnswers.push_back("Yes");
    question1.possibleAnswers.push_back("No");
    question1.possibleAnswers.push_back("Maybe");
    question1.correctAnswer = 0;
    auto question2 = Question();
    question2.text = "Are you sure?";
    question2.possibleAnswers.push_back("Yes");
    question2.possibleAnswers.push_back("No");
    question2.correctAnswer = 0;
    tests["1"].questions.push_back(question1);
    tests["1"].questions.push_back(question2);
    tests.insert(std::make_pair("2", Test()));
    tests["2"].descriprion = "Just kek";
    auto question3 = Question();
    question3.text = "Kek?";
    question3.possibleAnswers.push_back("Lol");
    question3.possibleAnswers.push_back("Cheburek");
    question3.possibleAnswers.push_back("Wow");
    question3.correctAnswer = 1;
    tests["2"].questions.push_back(question3);
}
