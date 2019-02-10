//
// Created by karvozavr on 07/02/19.
//

#include "server.hpp"

void term_handler(int signum) {
    done = 1;
}
