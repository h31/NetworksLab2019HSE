#pragma once

#include <thread>
#include <unordered_map>
#include <map>
#include <mutex>

class socket_pool {
public:
    void insert(int socket_descriptor, std::thread* thread);
    void remove(int socket_descriptor);
    void clear();
private:
    std::mutex lock;
    std::unordered_map<int, std::thread*> pool;
};

class socket_int_pool {
public:
    void insert(int socket_descriptor, int id, std::thread* thread);
    void remove(int socket_descriptor, int id);
    void clear();
private:
    std::mutex lock;
    std::map<std::pair<int, int>, std::thread*> pool;
};
