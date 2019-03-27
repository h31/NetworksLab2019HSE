#include <utility>

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

    enum ClientStatus {
        NEW,
        FREELANCER,
        CUSTOMER,
        FINISH,
        UNCHANGED
    };

    class Order {
    public:
        enum State {
            OPEN,
            ASSIGNED,
            IN_PROGRESS,
            PENDING,
            DONE
        };

        explicit Order(std::string customer, std::string description)
                : customer(std::move(customer)),
                  description(std::move(description)),
                  task_id(task_counter++),
                  state(OPEN) {}

        const int task_id;
        const std::string description;
        const std::string customer;
        State state;
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

    void UserInteractionLoop(int sock_fd) override;

    MarketServer::ClientStatus
    WorkWithUnauthorized(int sock_fd, const Message &message, Freelancer **freelancer, Customer **customer);

    MarketServer::ClientStatus WorkWithFreelancer(Freelancer *freelancer, const Message &message);

    MarketServer::ClientStatus WorkWithCustomer(Customer *customer, const Message &message);

    Message LookupOrdersOf(Customer *customer);

    Message LookupOpenOrders();

    void DeleteCustomer(Customer *customer);

    void DeleteFreelancer(Freelancer *freelancer);

    std::string StateToString(Order::State state);

    std::string WorkersToString(std::set<std::string> set);

    void OrderToString(std::string &result, Order *o);
};

#endif //SERVER_SERVER_H
