//
// Created by iisuslik on 17.02.19.
//

#include "../include/client.h"

void println(std::string s) {
    std::cout << s << std::endl;
}

void print(std::string s) {
    std::cout << s << std::flush;
}

std::string readLine() {
    std::string res;
    getline(std::cin, res);
    return res;
}

Client::Client(const std::string &host, uint16_t port) : host(host), port(port) {

    /* Create a socket point */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
}

void Client::start() {
    server = gethostbyname(host.c_str());

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
        exit(1);
    }
    workingCycle();

}

Client::~Client() {
    println("Bye!!!");
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
}

void Client::workingCycle() {
    println(help);
    while (!stop) {
        std::string request = getRequest();
        if (request.empty() || (request == "HELP")) {
            continue;
        }
        request = request + '\n';
        if (!sendRequest(request)) {
            break;
        }
        getResponse();
    }
}

std::string Client::getRequest() {
    std::string request = readLine();
    if (request == "regist") {
        print("Type your new login: ");
        std::string login = readLine();
        print("Are you a tester or developer?(test/dev): ");
        status = readLine();
        if (status == "dev") {
            return "REGD " + login;
        } else if (status == "test") {
            return "REGT " + login;
        } else {
            println("Wrong status: " + status);
            status = "";
        }
    } else if (request == "auth") {
        print("Type your login: ");
        std::string login = readLine();
        return "AUTH " + login;
    } else if (request == "exit") {
        stop = true;
        return "";
    } else if (request == "help") {
        print(help);
        if (status == "dev") {
            print(helpDev);
        } else if (status == "test") {
            print(helpTester);
        }
        return "HELP";
    } else {
        if (status.empty()) {
            println("Pleas auth to run other commands");
        } else if (request == "closed") {
            if (status == "dev") {
                println("You aren't a tester, it won't work!!!");
                return "";
            }
            return "BCLS ";
        } else if (request == "open") {
            if (status == "dev") {
                return "LIST ";
            } else if (status == "test") {
                return "BACT ";
            }
        } else if (request == "add") {
            if (status == "dev") {
                println("You aren't a tester, it won't work!!!");
                return "";
            }
            print("Type new bug id: ");
            std::string bugId = readLine();
            print("Type your project id: ");
            std::string projectId = readLine();
            print("Type bug description: ");
            std::string bugText = readLine();
            return "BREP " + login + '|' + projectId + '|' + bugId + '|' + bugText + '|';
        } else if (request == "accept") {
            if (status == "dev") {
                println("You aren't a tester, it won't work!!!");
                return "";
            }
            print("Type bug id: ");
            std::string bugId = readLine();
            return "BREV " + bugId + '|' + "accept|";
        } else if (request == "reject") {
            if (status == "dev") {
                println("You aren't a tester, it won't work!!!");
                return "";
            }
            print("Type bug id: ");
            std::string bugId = readLine();
            return "BREV " + bugId + '|' + "reject|";
        } else if (request == "fix") {
            if (status == "test") {
                println("You aren't a developer, it won't work!!!");
                return "";
            }
            print("Type bug id: ");
            std::string bugId = readLine();
            return "BFIX " + bugId;
        } else {
            std::cout << "Unknown command: " << request << std::endl;
        }
    }
    return "";
}

bool Client::sendRequest(std::string request) {
    int n = write(sockfd, request.c_str(), request.size());
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
    return n != 0;
}

void Client::getResponse() {
    std::string response = "";
    char buffer[BUFFER_SIZE];
    while (!stop) {
        ssize_t n = read(sockfd, buffer, BUFFER_SIZE - 1);
        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }
        if (n == 0) {
            stop = true;
            return;
        }
        for (ssize_t i = 0; i < n; i++) {
            if (buffer[i] == SPLIT) {
                handleResponse(response);
                return;
            } else {
                response += buffer[i];
            }
        }
    }
}

void Client::handleResponse(std::string response) {
    if (response.length() < 8 || response[4] != ' ' || response[7] != ' ') {
        println("Server error");
        return;
    }
    std::string commandCode = response.substr(0, 4);
    std::string errorCode = response.substr(5, 2);
    if (errorCode != "OK") {
        println("Incorrect request to server: " + errorCode);
        if (commandCode == "REGI") {
            status = "";
        }
        return;
    } else {
        println("Success!!!");
        if (commandCode == "AUTH") {
            std::string maybeStatus = response.substr(8, response.length() - 8);
            if (maybeStatus == "dev") {
                status = "dev";
            } else if (maybeStatus == "qa") {
                status = "test";
            } else {
                println("Server error auth");
            }
        } else if (commandCode == "LIST" || commandCode == "BCLS" || commandCode == "BACT") {
            printBugList(response.substr(8, response.length() - 8));
        }
    }
}

void Client::printBugList(std::string list) {
    println("BUG LIST");
}
