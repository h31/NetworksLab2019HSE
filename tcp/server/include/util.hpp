#pragma once

#include <iostream>

void log(const std::string &s) {
    std::cout << s << std::endl;
}

void log_error(const std::string &s) {
    std::cerr << "Error: " << s << std::endl;
}