#ifndef BUG_TRACKING_SERVER_USER_H
#define BUG_TRACKING_SERVER_USER_H


struct User {
    enum Role {
        NONE, DEVELOPER, TESTER
    };

    int id = -1;
    Role role = Role::NONE;
};


#endif
