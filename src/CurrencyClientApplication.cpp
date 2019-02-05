//
// Created by mikhail on 03.02.19.
//

#include <include/CurrencyClientApplication.h>
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "usage: .%s <hostname> <port>\n", argv[0]);
        return 0;
    }
    std::string hostname = argv[1];
    auto portno = static_cast<uint16_t>(strtol(argv[2], nullptr, 10));
    CurrencyClientApplication(hostname, portno).run();
    return 0;
}

void CurrencyClientApplication::run() {
    printUsage();

    bool toExit = false;
    while (!toExit) {
        std::string command;
        std::cout << ">";
        std::cin >> command;
        if (command == "list") {
            auto currencies = client.list();
            std::cout << "RESPONSE:\n";
            for (const auto &currency : currencies) {
                std::cout << currency.get_name() << " " << currency.get_absolute_change() <<
                          " " << currency.get_relative_change() << "\n";
            }
        } else if (command == "exit") {
            toExit = true;
        }
    }
}

void CurrencyClientApplication::printUsage() {
    std::cout << "Commands:\n"
    << "list - list currencies with rate and rate differences\n"
    << "addCurrency <currency name> <current rate> - adds new currency\n"
    << "remove <currency name>\n"
    << "addRate <currency name> <new rate> - adds new rate for given currency\n"
    << "getCurrencyHistory <currency name> - returns rate history of given currency\n"
    << "exit - to exit client application\n"
    << "\n";
}

CurrencyClientApplication::CurrencyClientApplication(const std::string &host, uint16_t portno) : host(host),
                                                                                                 portno(portno) {}