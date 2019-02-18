#include <fstream>
#include <iostream>
#include "../include/UserService.h"

UserService::UserService(std::string filename) {
    std::ifstream in;
    in.open(filename);
    if (!in) {
        std::cerr << "File was not found\n";
        return;
    }
    int id, role;
    while (!in.eof()) {
        in >> id >> role;
        _users[id] = User();
        _users[id].id = id;
        _users[id].role = static_cast<User::Role>(role);
    }
    in.close();
}

User UserService::getUser(int id) {
    User user;
    _users_mutex.lock();
    if (_users.find(id) == _users.end()) {
        user = _users[id];
    } else {
        user = User();
    }
    _users_mutex.unlock();
    return user;
}