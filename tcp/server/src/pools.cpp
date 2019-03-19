#include <pools.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

void socket_pool::insert(int socket_descriptor, std::thread *thread) {
    lock.lock();
    pool[socket_descriptor] = thread;
    lock.unlock();
}

void socket_pool::remove(int socket_descriptor) {
    lock.lock();
    pool.erase(socket_descriptor);
    lock.unlock();
}

void socket_pool::clear() {
    for (auto socket_thread_pair : this->pool) {
        int socket_descriptor = socket_thread_pair.first;
        close(socket_descriptor);
        std::thread* client_thread = socket_thread_pair.second;
        client_thread->join();
    }
    this->pool.clear();
}

void socket_int_pool::insert(int socket_descriptor, int id, std::thread *thread) {
    lock.lock();
    pool[{socket_descriptor, id}] = thread;
    lock.unlock();
}

void socket_int_pool::remove(int socket_descriptor, int id) {
    lock.lock();
    pool.erase({socket_descriptor, id});
    lock.unlock();
}

void socket_int_pool::clear() {
    for (auto socket_id_thread : this->pool) {
        auto socket_and_id = socket_id_thread.first;
        close(socket_and_id.first);
        std::thread* slow_op_thread = socket_id_thread.second;
        slow_op_thread->join();
    }
    this->pool.clear();
}
