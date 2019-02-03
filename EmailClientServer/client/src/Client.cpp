#include <Client.h>
#include <iostream>
#include <communication/communication.h>
#include <model/request.h>
#include <model/response.h>
#include <serialization/response.h>

#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

using namespace communication;
using namespace model;
using namespace request;
using namespace response;
using namespace serialization;
using namespace std;


Client::Client(const std::string &hostEmail, const std::string &serverAddress, uint16_t port)
    : hostEmail(hostEmail)
    , port(port)
    , serverAddress(serverAddress)
    , commands({
        {"send", make_shared<SendEmailCommand>(this)},
        {"check", make_shared<CheckEmailCommand>(this)},
        {"get", make_shared<GetEmailCommand>(this)},
        {"help", make_shared<PrintHelpCommand>()}
       }) {}

void Client::run() {
    while (true) {
        cout << ":>  ";
        string command;
        cin >> command;
        if (command == "exit") {
            cout << "Good buy" << endl;
            break;
        }
        if (commands.find(command) == commands.end()) {
            command = "help";
        }
        Command *pCommand = commands[command].get();
        pCommand->execute();
    }
}

Client::SendEmailCommand::SendEmailCommand(Client *client) : client(client) {}

void Client::SendEmailCommand::execute() {
    cout << "enter recipient email :> ";
    string recipient, theme, body;
    cin >> recipient;
    cout << "enter email theme :> ";
    cin >> theme;
    cout << "enter email body :> ";
    cin >> body;
    Email email(theme, body, client->hostEmail, recipient);
    int socket = -1;
    try {
        socket = client->openSocket();
        sendRequest(socket, make_unique<SendEmailRequest>(client->hostEmail, email));
        auto buffer = receiveMessage(socket);
        ResponseDeserializer deserializer(buffer.get());
        auto response = deserializer.parseSendEmailResponse();
        if (response->isError()) {
            client->processBadResponse(response->getResponseBody());
        } else {
            cout << "Message sent" << endl;
        }
    } catch (const char * msg) {
        // TODO
    }
    close(socket);
}

Client::CheckEmailCommand::CheckEmailCommand(Client *client) : client(client) {}

void Client::CheckEmailCommand::execute() {
    int socket = -1;
    try {
        socket = client->openSocket();
        sendRequest(socket, make_unique<CheckEmailRequest>(client->hostEmail));
        auto buffer = receiveMessage(socket);
        ResponseDeserializer deserializer(buffer.get());
        auto response = deserializer.parseCheckEmailResponse();
        if (response->isError()) {
            client->processBadResponse(response->getResponseBody());
        } else {
            const auto *body = castResponseBody<EmailInfosResponseBody>(response->getResponseBody());
            const vector<EmailInfo> &infos = body->getInfos();
            if (infos.empty()) {
                cout << "No unread emails" << endl;
            } else {
                for (const auto &info: infos) {
                    cout << "ID: " << info.getId() << endl;
                    cout << "From: " << info.getAuthor() << endl;
                    cout << "Theme: " << info.getTheme() << endl;
                    cout << endl;
                }
            }
        }
    } catch (const char * msg) {
        // TODO
    }
    close(socket);
}

Client::GetEmailCommand::GetEmailCommand(Client *client) : client(client) {}

void Client::GetEmailCommand::execute() {
    uint32_t id;
    cout << "Enter id :> ";
    cin >> id;
    int socket = -1;
    try {
        socket = client->openSocket();
        sendRequest(socket, make_unique<GetEmailRequest>(client->hostEmail, id));
        auto buffer = receiveMessage(socket);
        ResponseDeserializer deserializer(buffer.get());
        auto response = deserializer.parseGetEmailResponse();
        if (response->isError()) {
            client->processBadResponse(response->getResponseBody());
        } else {
            const auto *body = castResponseBody<EmailResponseBody>(response->getResponseBody());
            const Email &email = body->getEmail();
            cout << "From: " << email.getAuthor() << endl;
            cout << "Theme: " << email.getTheme() << endl;
            cout << "Body: " << email.getBody() << endl;
            cout << endl;
        }
    } catch (const char * msg) {
        // TODO
    }
    close(socket);
}

int Client::openSocket() {
    struct sockaddr_in address;
    int sock = 0;
    ssize_t valread;
    struct sockaddr_in serv_addr;

    std::string hello = "Hello from client ";
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, serverAddress.c_str(), &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    return sock;
}

void Client::processBadResponse(const response::ResponseBody &responseBody) {
    const auto *body = castResponseBody<ErrorResponseBody>(responseBody);
    cout << "Something went wrong: " << body->getMessage() << endl;
    cout << endl;
}

template<class T>
const T *castResponseBody(const response::ResponseBody &responseBody) {
    return reinterpret_cast<const T*>(&responseBody);
}

void Client::PrintHelpCommand::execute() {
    cout << "Available commands:" << endl;
    cout << "  send" << endl;
    cout << "  check" << endl;
    cout << "  get" << endl;
    cout << "  help" << endl;
    cout << "  exit" << endl;
    cout << endl;
}
