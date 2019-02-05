#include <netinet/in.h>
#include <Server.h>
#include <iostream>
#include <thread>

#include "serialization/request.h"
#include "serialization/response.h"
#include "communication/communication.h"

using namespace util;
using namespace std;
using namespace model;
using namespace request;
using namespace response;
using namespace serialization;

Server::Server(uint16_t port) : port(port), clientIdCounter(0), idCounter(0), emails(), sockets() {}

void Server::runServer() {
    int serverSocket, clientSocket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    sockets.insert(serverSocket);

    // Forcefully attaching socket to the port 8080
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(serverSocket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true) {
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }
        uint32_t clientId = clientIdCounter++;
        cout << "Client " << clientId << " connected" << endl;
        sockets.insert(clientSocket);
        Worker worker(clientSocket, clientId, *this);
        thread thr(worker);
        thr.detach();
    }
#pragma clang diagnostic pop
}

void Server::shutdown() {
    for (int socket : sockets) {
        close(socket);
    }
}

Server::Worker::Worker(int socket, uint32_t clientId, Server &server) : socket(socket), clientId(clientId), server(server) {}

void Server::Worker::operator()() {
    try {
        auto buffer = receiveMessage(socket);
        RequestDeserializer deserializer(buffer.get());
        auto request = deserializer.parseRequest();
        request->accept(this);
    } catch (const char *msg) {
        perror(msg);
    }
    close(socket);
}

void Server::Worker::visitSendEmailRequest(const SendEmailRequest *request) {
    const Email &email = request->getEmail();
    uint32_t id = server.idCounter++;
    EmailWithInfo emailWithInfo(id, email);
    server.emails[id] = emailWithInfo;
    auto responseBody = make_shared<EmptyResponseBody>();
    sendResponse(socket, make_unique<Response>(responseBody));
}

void Server::Worker::visitCheckEmailRequest(const CheckEmailRequest *request) {
    auto &author = request->getAuthor();
    vector<EmailInfo> uncheckedEmails;
    for (const auto &entry : server.emails) {
        auto &info = entry.second;
        if (!info.isChecked() && info.getEmail().getRecipient() == author) {
            uncheckedEmails.push_back(info.getInfo());
        }
    }
    cout << uncheckedEmails.size() << endl;
    auto responseBody = make_shared<EmailInfosResponseBody>(uncheckedEmails);
    sendResponse(socket, make_unique<Response>(responseBody));
}

void Server::Worker::visitGetEmailRequest(const GetEmailRequest *request) {
#define ERROR_MESSAGE "Email was not founded"

    shared_ptr<ResponseBody> responseBody;
    try {
        uint32_t id = request->getId();
        size_t allEmails = server.emails.size();
        if (id >= allEmails) {
            throw ERROR_MESSAGE;
        }
        EmailWithInfo &info = server.emails[id];
        auto &email = info.getEmail();
        if (email.getAuthor() != request->getAuthor()) {
            throw ERROR_MESSAGE;
        }
        info.checkEmail();
        responseBody = make_shared<EmailResponseBody>(email);
    } catch (const char * msg) {
        responseBody = make_shared<ErrorResponseBody>(msg);
    }
    sendResponse(socket, make_unique<Response>(responseBody));

#undef ERROR_MESSAGE
}
