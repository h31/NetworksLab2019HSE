#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <memory>
#include <string>
#include <unordered_map>
#include <model/response.h>

class Command {
public:
    virtual void execute() = 0;
    virtual ~Command() = default;
};

class Client {
private:
    const std::string hostEmail;
    const std::string serverAddress;
    const uint16_t port;
    std::unordered_map<std::string, std::shared_ptr<Command>> commands;
public:
    Client(const std::string &hostEmail, const std::string &serverAddress, uint16_t port);

    void run();
private:
    int openSocket();

    void processBadResponse(const response::ResponseBody &responseBody);

    class SendEmailCommand : public Command {
    private:
        Client *client;
    public:
        explicit SendEmailCommand(Client *client);
        void execute() override;
    };

    class CheckEmailCommand : public Command {
    private:
        Client *client;
    public:
        explicit CheckEmailCommand(Client *client);
        void execute() override;
    };

    class GetEmailCommand : public Command {
    private:
        Client *client;
    public:
        explicit GetEmailCommand(Client *client);
        void execute() override;
    };

    class PrintHelpCommand : public Command {
        void execute() override;
    };
};

template <class T>
const T* castResponseBody(const response::ResponseBody &responseBody);

#endif //CLIENT_CLIENT_H
