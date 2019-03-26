#include <pools.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

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


bool operator <(const struct sockaddr_in& x, const struct sockaddr_in& y) {
    return x.sin_addr.s_addr < y.sin_addr.s_addr || x.sin_port < y.sin_port;
}

void long_computation_response_pool::insert(sockaddr_in client_addr, int id, int type, dctp_response_header response) {
    lock.lock();
    pool[{client_addr, id, type}] = response;
    lock.unlock();
}

dctp_response_header long_computation_response_pool::get(struct sockaddr_in client_addr, int id, int type) {
    dctp_response_header response{};
    lock.lock();
    response = pool[{client_addr, id, type}];
    lock.unlock();
    return response;
}

void long_computation_response_pool::clear() {
    this->pool.clear();
}

bool long_computation_response_pool::contains(struct sockaddr_in client_addr, int id, int type) {
    bool result;
    lock.lock();
    result = pool.find(std::make_tuple(client_addr, id, type)) != pool.end();
    lock.unlock();
    return result;
}
