#ifndef BUG_TRACKING_SERVER_USERS_SERVER_H
#define BUG_TRACKING_SERVER_USERS_SERVER_H

#include <map>
#include <mutex>

class UserService {
public:
    UserService();

    class User {
        public:
            User();
            User(int id, int role);

            int id;
            int role;

            bool isEmpty();

            static User emptyUser();
    };

    User registerNewUser(int id, int role);

    User getUser(int id);

private:
    std::map<int, User> _users;
    std::mutex _users_mutex;
};

#endif
