#pragma once

#include <thread>
#include <unordered_map>
#include <map>
#include <mutex>
#include <netinet/in.h>
#include "response.hpp"

class socket_int_pool {
public:
    void insert(int socket_descriptor, int id, std::thread* thread);
    void remove(int socket_descriptor, int id);
    void clear();
private:
    std::mutex lock;
    std::map<std::pair<int, int>, std::thread*> pool;
};

bool operator <(const struct sockaddr_in& x, const struct sockaddr_in& y);

class long_computation_response_pool {
public:
    void insert(struct sockaddr_in client_addr, int id, int type, dctp_response_header response);
    dctp_response_header get(struct sockaddr_in client_addr, int id, int type);
    bool contains(struct sockaddr_in client_addr, int id, int type);
    void clear();
private:
    std::mutex lock;
    std::map<std::tuple<struct sockaddr_in, int, int>, dctp_response_header> pool;
};
