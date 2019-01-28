#include "../include/Server.h"
#include <netinet/in.h>
#include "serialization/request.h"
#include "serialization/response.h"

using namespace model;
using namespace request;
using namespace response;
using namespace serialization;
using namespace std;

Server::Server(uint16_t port) : thread_pool(), port(port), idCounter(0), emails() {}

void Server::run_server() {
    int serverFd, clientSocket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(serverFd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true) {
        if ((clientSocket = accept(serverFd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }
        thread_pool.push(Worker(clientSocket, *this));
    }
#pragma clang diagnostic pop
}

Server::Worker::Worker(int socket, Server &server) : socket(socket), server(server) {}

void Server::Worker::operator()(int id) {
    try {
        uint32_t messageSize = readMessageSize();
        unique_ptr<uint8_t[]> buffer(new uint8_t[messageSize]);
        readToBuffer(buffer.get(), messageSize);
        RequestDeserializer deserializer(buffer.get());
        auto request = deserializer.parseRequest();
        request->accept(this);
    } catch (const char *msg) {
        perror(msg);
    }
}

void Server::Worker::readToBuffer(uint8_t *buffer, size_t size) {
    ssize_t valread = read(socket, buffer, size);
    if (valread < 0) {
        throw "Can't read from socket";
    }
}

uint32_t Server::Worker::readMessageSize() {
    uint8_t messageSizeBuffer[4];
    readToBuffer(messageSizeBuffer, 4);
    DataDeserializer dataDeserializer(messageSizeBuffer);
    return dataDeserializer.parseUInt32();
}

void Server::Worker::visitSendEmailRequest(const SendEmailRequest *request) {
    const Email &email = request->getEmail();
    uint32_t id = server.idCounter++;
    EmailWithInfo emailWithInfo(id, email);
    auto responseBody = make_unique<EmptyResponseBody>();
    sendResponse(make_unique<Response>(responseBody.get()));
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
    auto responseBody = make_unique<EmailInfosResponseBody>(uncheckedEmails);
    sendResponse(make_unique<Response>(responseBody.get()));
}

void Server::Worker::visitGetEmailRequest(const GetEmailRequest *request) {
#define ERROR_MESSAGE "Email was not founded"

    unique_ptr<ResponseBody> responseBody;
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
        responseBody = make_unique<EmailResponseBody>(email);
    } catch (const char * msg) {
        responseBody = make_unique<ErrorResponseBody>(msg);
    }
    sendResponse(make_unique<Response>(responseBody.get()));
}

void Server::Worker::sendResponse(std::unique_ptr<response::Response> &&response) {
    ResponseSerializer serializer(response.get());
    auto pair = serializer.serialize();
    uint32_t messageSize = pair.first;
    shared_ptr<uint8_t[]> &buffer = pair.second;
    send(socket, buffer.get(), messageSize, 0);
}
