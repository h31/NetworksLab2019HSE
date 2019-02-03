//
// Created by mikhail on 03.02.19.
//

#include <include/CurrencyClientApplication.h>
#include <string>
#include <iostream>

#include "include/CurrencyClientApplication.h"

int main() {
    CurrencyClientApplication().run();
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
