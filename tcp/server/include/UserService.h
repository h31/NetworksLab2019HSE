#ifndef BUG_TRACKING_SERVER_USERS_SERVER_H
#define BUG_TRACKING_SERVER_USERS_SERVER_H

#include <map>
#include <mutex>
#include "User.h"

class UserService {
public:
    explicit UserService(std::string filename);

    User getUser(int id);

private:
    std::map<int, User> _users;
    std::mutex _users_mutex;
};

#endif
