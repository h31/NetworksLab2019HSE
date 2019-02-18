#include "../include/UserService.h"

UserService::UserService() = default;

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

UserService::User::User(int id, Role role) : id(id), role(role) {}

UserService::User::User() : User(-1, Role::NONE) {}

bool UserService::User::isEmpty() {
    return id == -1 || role == Role::NONE;
}
