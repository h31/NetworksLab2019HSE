#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <map>
#include <set>
#include <boost/thread/executors/basic_thread_pool.hpp>
#include "../include/message.h"
#include "tcp_server.h"

class MarketServer : public TcpServer {
public:
    MarketServer();

    ~MarketServer();

    bool BanUser(const std::string &name);

private:
    class Freelancer {
    public:
        explicit Freelancer(const std::string &name, int socket_fd);

        ~Freelancer();

        const int socket_fd;
        const std::string name;
        std::thread writer;
        boost::sync_queue<Message> messages_;
    };

    class Customer {
    public:
        explicit Customer(const std::string &name, int socket_fd);

        ~Customer();

        const int socket_fd;
        const std::string name;
        std::thread writer;
        boost::sync_queue<Message> messages_;
    };

    class Order {
    public:
        enum State {
            OPEN,
            IN_PROGRESS,
            DONE
        };

        explicit Order(const std::string &customer, const std::string &description)
                : customer(customer), description(description), task_id(task_counter++), state(OPEN) {}

        const int task_id;
        const std::string description;
        const std::string customer;
        State state;
        std::mutex workers_mutex;
        std::set<std::string> workers;

    private:
        static int task_counter;
    };

    std::map<std::string, Freelancer *> freelancers;
    std::map<std::string, Customer *> customers;
    std::map<int, Order *> orders;

    std::mutex customers_mutex_;
    std::mutex freelancers_mutex_;

    std::shared_timed_mutex orders_mutex_;

    Message ProcessStartDraw();

    Message ProcessEndDraw();

    void StartWorkingWithClient(int sock_fd) override;

    void WorkWithFreelancer(Freelancer *freelancer);

    void WorkWithCustomer(Customer *customer);

    Message LookupOrdersOf(Customer *customer);

    Message LookupOpenOrders();

    Message ProcessBet(Freelancer &player, std::string bet);

    void DeleteCustomer(Customer *customer);

    void DeleteFreelancer(Freelancer *freelancer);

    std::string StateToString(Order::State state);

    std::string WorkersToString(std::set<std::string> set);

    void OrderToString(std::string &result, Order *o);
};

#endif //SERVER_SERVER_H
