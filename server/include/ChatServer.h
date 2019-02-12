//
// Created by Владислав Калинин on 11/02/2019.
//

#ifndef NETWORKS_SERVER_H
#define NETWORKS_SERVER_H


#include <netinet/in.h>
#include "../../common/include/ctpl.h"
#include <tbb/concurrent_unordered_map.h>

struct User;

class ChatServer {

public:
    ChatServer(const char *hostname, uint16_t port);

    void run();

private:
    int master_socket;
    sockaddr_in address;
    ctpl::thread_pool thread_pool;
    tbb::concurrent_unordered_map<int, User> online_users;
};

struct User {
    User(std::string name_, bool is_admin_) : name(name_), is_admin(is_admin_) {}

    void kick() {
        was_kicked = true;
    }

    std::string name;
    bool is_admin;
    bool was_kicked = false;
};

#endif //NETWORKS_SERVER_H
