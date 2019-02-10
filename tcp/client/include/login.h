#ifndef CLIENT_LOGIN_H
#define CLIENT_LOGIN_H

struct Identifier {
    Identifier(std::string& login, std::string& password) : login(login), password(password) {}
    Identifier() = default;

    std::string login = "";
    std::string password = "";
};

std::string has_account();

Identifier read_login_and_password();
Identifier login(int socket_descriptor);

std::string create_password();
Identifier registration(int socket_descriptor);

#endif //CLIENT_LOGIN_H
