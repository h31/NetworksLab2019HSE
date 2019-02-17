//
// Created by mikhail on 03.02.19.
//

#include <CurrencyClientApplication.h>
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
        } else if (command == "addCurrency") {
            std::string currency_name;
            int32_t currency_rate;
            std::cin >> currency_name >> currency_rate;
            if (client.addCurrency(Currency(currency_name, currency_rate))) {
                std::cout << "Done\n";
            } else {
                std::cout << "Currency already exists\n";
            }
        } else if (command == "removeCurrency") {
            std::string currency_name;
            std::cin >> currency_name;
            if (client.remove(Currency(currency_name))) {
                std::cout << "Done\n";
            } else {
                std::cout << "Currency doesn't exist\n";
            }
        } else if (command == "addRate") {
            std::string currency_name;
            int32_t new_rate;
            std::cin >> currency_name >> new_rate;
            if (client.addRate(Currency(currency_name), new_rate)) {
                std::cout << "Done\n";
            } else {
                std::cout << "Currency doesn't exist\n";
            }
        } else if (command == "getCurrencyHistory") {
            std::string currency_name;
            std::cin >> currency_name;
            Currency currency_with_history = client.getCurrencyWithHistory(Currency(currency_name));
            std::cout << "RATES: ";
            for (auto rate : currency_with_history.get_rates()) {
                std::cout << rate << " ";
            }
            std::cout << "\n";
        } else if (command == "exit") {
                toExit = true;
        } else {
            std::cout << "UNKNOWN COMMAND\n";
        }
    }
}

void CurrencyClientApplication::printUsage() {
    std::cout << "Commands:\n"
    << "list - list currencies with rate and rate differences\n"
    << "addCurrency <currency name> <current rate> - adds new currency\n"
    << "removeCurrency <currency name>\n"
    << "addRate <currency name> <new rate> - adds new rate for given currency\n"
    << "getCurrencyHistory <currency name> - returns rate history of given currency\n"
    << "exit - to exit client application\n"
    << "\n";
}

CurrencyClientApplication::CurrencyClientApplication(const std::string &host, uint16_t portno) : host(host),
                                                                                                 portno(portno) {}