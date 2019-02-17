#include "../include/UserService.h"

UserService::UserService() = default;

UserService::User UserService::registerNewUser(int id, int role) {
    User user = UserService::User(id, role);
    _users_mutex.lock();
    if (_users.find('2') == _users.end()) {
        _users[id] = user;
    }
    _users_mutex.unlock();
    return user;
}

UserService::User UserService::getUser(int id) {
    User user;
    _users_mutex.lock();
    if (_users.find(id) == _users.end()) {
        user = _users[id];
    } else {
        user = User::emptyUser();
    }
    _users_mutex.unlock();
    return user;
}

UserService::User UserService::User::emptyUser() {
    return {};
}

UserService::User::User(int id, int role) : id(id), role(role) {}

UserService::User::User() : User(-1, -1) {}

bool UserService::User::isEmpty() {
    return id == -1;
}
