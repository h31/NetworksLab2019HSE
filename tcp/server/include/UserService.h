#ifndef BUG_TRACKING_SERVER_USERS_SERVER_H
#define BUG_TRACKING_SERVER_USERS_SERVER_H

#include <map>
#include <mutex>

class UserService {
public:
    UserService();

    class User {
        public:
            enum Role {
                NONE, DEVELOPER, TESTER
            };

            User();
            User(int id, Role role);

            int id;
            Role role;

            bool isEmpty();

            static User emptyUser();
    };

    User getUser(int id);

private:
    std::map<int, User> _users;
    std::mutex _users_mutex;
};

#endif
