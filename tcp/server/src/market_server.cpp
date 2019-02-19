#include <sys/socket.h>
#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include <thread>

#include "../include/market_server.h"

int MarketServer::Order::task_counter = 0;

void MarketServer::StartWorkingWithClient(int sock_fd) {
    std::function<void()> workWithClient = nullptr;
    while (true) {
        Message message = Message::Read(sock_fd);
        Message::Type ans_type;
        std::string &name = message.body;
        if (Message::NEW_CUSTOMER == message.type) {
            customers_mutex_.lock();
            if (customers.count(name) == 0 and freelancers.count(name) == 0) {
                auto *customer = new Customer(name, sock_fd);
                customers[name] = customer;
                workWithClient = std::bind(&MarketServer::WorkWithCustomer, this, customer);
                ans_type = Message::CUSTOMER_ADDED;
                std::cout << "New customer\n";
            } else {
                ans_type = Message::CANT_ADD_CUSTOMER;
            }
            customers_mutex_.unlock();
        } else if (Message::NEW_FREELANCER == message.type) {
            freelancers_mutex_.lock();
            if (freelancers.count(name) == 0 and customers.count(name) == 0) {
                auto *freelancer = new Freelancer(name, sock_fd);
                freelancers[name] = freelancer;
                workWithClient = std::bind(&MarketServer::WorkWithFreelancer, this, freelancer);
                ans_type = Message::FREELANCER_ADDED;
                std::cout << "New freelancer\n";
            } else {
                ans_type = Message::CANT_ADD_FREELANCER;
            }
            freelancers_mutex_.unlock();
        } else {
            ans_type = Message::UNAUTHORIZED;
        }

        Message ans_message(ans_type);
        ans_message.Write(sock_fd);

        if (workWithClient != nullptr) {
            break;
        }
    }
    workWithClient();
}

void MarketServer::WorkWithFreelancer(Freelancer *freelancer) {
    while (true) {
        Message message = Message::Read(freelancer->socket_fd);
        Message ans_message;
        switch (message.type) {
            case Message::GET_OPEN_ORDERS: {
                ans_message = LookupOpenOrders();
                break;
            }
            case Message::TAKE_ORDER: {
                orders_mutex_.lock();
                int id = stoi(message.body);
                if (orders.count(id) and orders[id]->state == Order::OPEN) {
                    orders[id]->workers.insert(freelancer->name);
                    ans_message.type = Message::TAKE_ORDER_SUCCESSFUL;
                } else {
                    ans_message = Message(Message::TAKE_ORDER_NOT_SUCCESSFUL, "no such open order");
                }
                orders_mutex_.unlock();
                break;
            }
            case Message::WORK_STARTED: {
                orders_mutex_.lock();
                int id = stoi(message.body);
                if (orders.count(id) and
                    orders[id]->state == Order::ASSIGNED and
                    orders[id]->workers.count(freelancer->name)) {
                    orders[id]->state = Order::IN_PROGRESS;
                    ans_message.type = Message::WORK_STARTED_SUCCESSFUL;
                } else {
                    ans_message.type = Message::WORK_STARTED_NOT_SUCCESSFUL;
                    ans_message.body = "no such work assigned to you";
                }
                orders_mutex_.unlock();
                break;
            }
            case Message::WORK_FINISHED: {
                orders_mutex_.lock();
                int id = stoi(message.body);
                if (orders.count(id) and
                    orders[id]->state == Order::IN_PROGRESS and
                    orders[id]->workers.count(freelancer->name)) {
                    orders[id]->state = Order::PENDING;
                    ans_message.type = Message::WORK_FINISHED_SUCCESSFUL;
                } else {
                    ans_message.type = Message::WORK_FINISHED_NOT_SUCCESSFUL;
                    ans_message.body = "no such work in progress assigned to you";
                }
                orders_mutex_.unlock();
                break;
            }
            case Message::UNDEFINED: {
                DeleteFreelancer(freelancer);
                std::cout << "Freelancer left\n";
                return;
            }

            default: {
                ans_message = Message(Message::INCORRECT_MESSAGE);
            }

        }
        freelancer->messages_.push(ans_message);
    }
}

void MarketServer::WorkWithCustomer(Customer *customer) {
    while (true) {
        Message message = Message::Read(customer->socket_fd);
        Message ans_message;
        switch (message.type) {
            case Message::NEW_ORDER: {
                orders_mutex_.lock();
                auto *order = new Order(customer->name, message.body);
                orders[order->task_id] = order;
                ans_message = Message(Message::ORDER_ACCEPTED, std::to_string(order->task_id));
                orders_mutex_.unlock_shared();
                break;
            }
            case Message::GET_MY_ORDERS: {
                ans_message = LookupOrdersOf(customer);
                break;
            }
            case Message::GET_OPEN_ORDERS: {
                ans_message = LookupOpenOrders();
                break;
            }
            case Message::GIVE_ORDER_TO_FREELANCER: {
                int id;
                char name[256];
                if (sscanf(message.body.c_str(), "%i %s", &id, name) == 2) {
                    orders_mutex_.lock();
                    if (orders.count(id) and
                        orders[id]->state == Order::OPEN and
                        orders[id]->customer == customer->name and
                        orders[id]->workers.count(name)) {
                        orders[id]->state = Order::ASSIGNED;
                        orders[id]->workers = {std::string(name)};
                        ans_message.type = Message::GIVE_ORDER_SUCCESSFUL;
                    } else {
                        ans_message.type = Message::GIVE_ORDER_NOT_SUCCESSFUL;
                        ans_message.body = "no such open order owned by you";
                    }
                    orders_mutex_.unlock();
                } else {
                    ans_message.type = Message::GIVE_ORDER_NOT_SUCCESSFUL;
                    ans_message.body = "parsing error: expected <int> <name> got " + message.body;
                }
                break;
            }
            case Message::WORK_ACCEPTED: {
                int id = std::stoi(message.body);
                orders_mutex_.lock();
                if (orders.count(id) and
                    orders[id]->state == Order::PENDING and
                    orders[id]->customer == customer->name) {
                    orders[id]->state = Order::DONE;
                    ans_message.type = Message::WORK_ACCEPTED_SUCCESSFUL;
                } else {
                    ans_message.type = Message::WORK_ACCEPTED_NOT_SUCCESSFUL;
                    ans_message.body = "no such work in progress owned by you";
                }
                orders_mutex_.unlock();
                break;
            }
            case Message::UNDEFINED: {
                DeleteCustomer(customer);
                std::cout << "Customer left\n";
                return;
            }

            default: {
                ans_message = Message(Message::INCORRECT_MESSAGE);
            }
        }
        customer->messages_.push(ans_message);
    }
}

Message MarketServer::LookupOpenOrders() {
    orders_mutex_.lock_shared();
    std::string result;
    for (auto &o: orders) {
        if (o.second->state == Order::OPEN) {
            OrderToString(result, o.second);
        }
    }
    orders_mutex_.unlock_shared();
    return Message(Message::LIST_OF_OPEN_ORDERS, result);
}

void MarketServer::OrderToString(std::string &result, MarketServer::Order *o) {
    result += std::to_string(o->task_id);
    result += " ";
    result += o->description;
    result += " ";
    result += StateToString(o->state);
    result += " ";
    result += WorkersToString(o->workers);
    result += '\n';
}

Message MarketServer::LookupOrdersOf(Customer *customer) {
    orders_mutex_.lock_shared();
    std::string result;
    for (auto &o: orders) {
        if (o.second->customer == customer->name) {
            OrderToString(result, o.second);
        }
    }
    orders_mutex_.unlock_shared();
    return Message(Message::LIST_OF_MY_ORDERS, result);
}

MarketServer::~MarketServer() {
    for (const auto &p : customers) {
        close(p.second->socket_fd);
    }
    for (const auto &p : freelancers) {
        close(p.second->socket_fd);
    }
    for (const auto &p : orders) {
        delete p.second;
    }
}

void MarketServer::DeleteCustomer(MarketServer::Customer *customer) {
    customers_mutex_.lock();
    customers.erase(customer->name);
    customers_mutex_.unlock();

    delete customer;
}

bool MarketServer::BanUser(const std::string &name) {
    bool result = false;
    freelancers_mutex_.lock();
    if (freelancers.count(name)) {
        DeleteFreelancer(freelancers[name]);
        result = true;
    }
    freelancers_mutex_.unlock();
    customers_mutex_.lock();
    if (customers.count(name)) {
        DeleteCustomer(customers[name]);
        result = true;
    }
    customers_mutex_.unlock();
    return result;
}

std::string MarketServer::StateToString(MarketServer::Order::State state) {
    switch (state) {
        case Order::OPEN:
            return "OPEN";
        case Order::IN_PROGRESS:
            return "IN_PROGRESS";
        case Order::DONE:
            return "DONE";
        case Order::ASSIGNED:
            return "ASSIGNED";
        case Order::PENDING:
            return "PENDING";
    }
}

std::string MarketServer::WorkersToString(std::set<std::string> set) {
    std::string result;
    for (auto &s : set)
        result += s + " ";
    return result;
}

void MarketServer::DeleteFreelancer(MarketServer::Freelancer *freelancer) {
    freelancers_mutex_.lock();
    customers.erase(freelancer->name);
    freelancers_mutex_.unlock();

    delete freelancer;

}

MarketServer::Freelancer::Freelancer(const std::string &name, int socket_fd) : name(name),
                                                                               socket_fd(socket_fd) {
    writer = std::thread([this]() {
        while (!messages_.closed()) {
            try {
                Message m = messages_.pull();
                m.Write(this->socket_fd);
            } catch (boost::sync_queue_is_closed &) {
                break;
            }
        }
    });
}

MarketServer::Customer::Customer(const std::string &name, int socket_fd) : name(name),
                                                                           socket_fd(socket_fd) {
    writer = std::thread([this]() {
        while (!messages_.closed()) {
            try {
                Message m = messages_.pull();
                m.Write(this->socket_fd);
            } catch (boost::sync_queue_is_closed &) {
                break;
            }
        }
    });
}

MarketServer::Freelancer::~Freelancer() {
    messages_.close();
    close(socket_fd);
    writer.join();
}

MarketServer::Customer::~Customer() {
    messages_.close();
    close(socket_fd);
    writer.join();
}
