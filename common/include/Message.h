//
// Created by Владислав Калинин on 11/02/2019.
//

#ifndef NETWORKS_MESSAGE_H
#define NETWORKS_MESSAGE_H

#include "json.hpp"

using namespace nlohmann;

class Message {
public:
    virtual std::string to_json_format()= 0;
};

class RegistryMessage : public Message {
public:
    RegistryMessage(std::string name_, std::string password_) : name(name_), password(password_) {}

    std::string to_json_format() override;

private:
    std::string name;
    std::string password;
};

class AuthenticationMessage : public Message {
public:
    AuthenticationMessage(std::string name_, std::string password_) : name(name_), password(password_) {}

    std::string to_json_format() override;

private:
    std::string name;
    std::string password;
};

class ResponseMessage : public Message {
public:
    ResponseMessage(int status_, std::string cause_) : status(status_), cause(cause_) {}

    ResponseMessage(int status_) : status(status_), cause("") {}

    std::string to_json_format() override;

private:
    int status;
    std::string cause;
};

class TextMessage : public Message {
public:
    TextMessage(std::string sender_, std::string message_) : sender(sender_), message(message_) {}

    std::string getSender() {
        return sender;
    }

    std::string getMessage() {
        return message;
    }

    std::string to_json_format() override;

protected:
    std::string sender;
    std::string message;
};

class PrivateMessage : public TextMessage {
public:
    PrivateMessage(std::string sender_, std::string reciever_, std::string message_) : TextMessage(sender_, message_),
                                                                                       reciever(reciever_) {}

    std::string to_json_format() override;

private:
    std::string reciever;
};

class KickMessage : public Message {
public:
    explicit KickMessage(std::string name_) : name(name_) {}

    std::string to_json_format() override;

private:
    std::string name;
};

#endif //NETWORKS_MESSAGE_H
