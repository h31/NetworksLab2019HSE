#include <string>
#include <iostream>
#include <limits>
#include <response.hpp>
#include <util.h>
#include "login.h"

std::string has_account() {
    std::cout << "Do you have an account?[y/n]\n";
    std::string answer;
    std::cin >> answer;

    while (answer != "y" && answer != "n" && !std::cin.eof()) {
        std::cout << "No, you're doing it wrong!\n";
        std::cout << "Write `y` if you have account; write `n` if you don't.\n";
        std::cin >> answer;
    }
    return answer;
}

Identifier read_login_and_password() {
    std::cout << "Enter your login and password: \n";

    std::string login_and_password;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, login_and_password);
    unsigned long delimiter = (login_and_password.find(' '));
    std::string login = login_and_password.substr(0, delimiter);
    std::string password = login_and_password.substr(delimiter + 1);

    return Identifier(login, password);
}

Identifier login(int socket_descriptor) {
    Identifier ident = read_login_and_password();
    struct pstp_check_login_request login_request(ident.login, ident.password);
    write_to_socket(socket_descriptor, (char *) &login_request, sizeof(login_request));

    struct pstp_response_header login_response_header;
    read_from_socket(socket_descriptor, (char *) &login_response_header, sizeof(login_response_header));

    if (login_response_header.code == INVALID_PASSWORD) {
        error("invalid combination of login-password!");
    } else if (login_response_header.code != OK) {
        error("unknown error.");
    }
}

std::string create_password() {
    std::cout << "Enter your password (max size - 63): \n";

    std::string password;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, password);

    return password;
}

Identifier registration(int socket_descriptor) {
    std::string password = create_password();
    struct pstp_register_request register_request(password);
    write_to_socket(socket_descriptor, (char *) &register_request, sizeof(register_request));

    struct pstp_response_header register_response_header;
    read_from_socket(socket_descriptor, (char *) &register_response_header, sizeof(register_response_header));

    if (register_response_header.code == INVALID_PASSWORD) {
        error("invalid password! (Perhaps, it was too big!)");
    } else if (register_response_header.code != OK) {
        error("unknown error.");
    }

    char buffer[TEXT_UNIT_SIZE];
    bzero(buffer, TEXT_UNIT_SIZE);
    read_from_socket(socket_descriptor, buffer, register_response_header.content_size);

    Identifier ident;
    ident.login = std::string(buffer);
    println("Your login: " + ident.login);
    ident.password = password;

    return ident;
}

