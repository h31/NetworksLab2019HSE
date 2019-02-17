#include <string>
#include <iostream>
#include "client.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Invalid arguments";
        exit(0);
    }
    int port_number = std::stoi(argv[2]);
    client cl(argv[1], port_number);
    while (true) {
        std::string command;
        std::cin >> command;
        if (command == "create_new_bug") {
            cl.addNewBug();
        }
        if (command == "register_user") {
            cl.registerUser();
        }
        if (command == "get_bugs") {
            cl.getBugsTester();
        }
        if (command == "get_developer_bugs") {
            cl.getBugsDeveloper();
        }
        if (command == "accept_bug") {
            cl.confirmBugTester();
        }
        if (command == "reject_bug") {
            cl.rejectBugTester();
        }
        if (command == "close_bug") {
            cl.closeBugDeveloper();
        }
        if (command == "quit") {
            cl.quit();
        }
    }
}