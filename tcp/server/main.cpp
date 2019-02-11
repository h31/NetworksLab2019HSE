#include <iostream>
#include "server.h"

int main() {
    std::vector<Test> tests = {{"first test", {{"2 + 2 =", "4"}, {"7 - 6 =", "1"}}}, {"second test", {}}};
    try {
        Server server(5003, tests);
        server.start();
    } catch (std::exception& e) {
        std::cerr << e.what() << "\n";
    }
    return 0;
}