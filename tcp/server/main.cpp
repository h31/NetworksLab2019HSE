#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <set>
#include <vector>
#include <map>
#include <iostream>

#define MAKE_ORDER_REQUEST 1
#define GET_ORDER_HISTORY_REQUEST 2
#define ADD_NEW_SHOP_REQUEST 3
#define SHUT_DOWN_CLIENT_REQUEST 4
#define ADD_CUSTOMER_REQUEST 5

class Request {
public:
    bool try_reading(int sd) {
        if (!read_header) {
            printf("Reading header for %d\n", sd);
            already_read += read(sd, buffer + already_read, 8 - already_read);
            if (already_read == 8) {
                read_header = true;
                request_type = *(uint32_t *)(buffer);
                request_type = *(uint32_t *)(buffer + sizeof(uint32_t));
                already_read = 0;
                printf("Done reading header for %d, request type is %d, client id %d\n", sd, request_type, id);
            }
        }
        if (read_header) {
            switch (request_type) {
                case SHUT_DOWN_CLIENT_REQUEST:
                case GET_ORDER_HISTORY_REQUEST:
                    return true;
                case MAKE_ORDER_REQUEST:
                case ADD_NEW_SHOP_REQUEST:
                    if (!reading_array) {
                        already_read += read(sd, buffer + already_read, 8 - already_read);
                        if (already_read == 8) {
                            reading_array = true;
                            (request_type == MAKE_ORDER_REQUEST ? shop_id : zone_id) = *(
                                    uint32_t *)(buffer);
                            number_of_goods = *(
                                    uint32_t *)(buffer + sizeof(uint32_t));
                            already_read = 0;
                        }
                    }
                    if (reading_array) {
                        while (goods.size() < number_of_goods){
                            already_read += read(sd, buffer + already_read, 4 - already_read);
                            if (already_read != 4) {
                                break;
                            }
                            goods.push_back(*(
                                    uint32_t *)buffer);
                            already_read = 0;
                        }
                        if (goods.size() == number_of_goods) {
                            return true;
                        }
                    }
                    return false;
                case ADD_CUSTOMER_REQUEST:
                    if (!read_customer_zone) {
                        already_read += read(sd, buffer + already_read, 4 - already_read);
                        if (already_read == 4) {
                            read_customer_zone = true;
                            zone_id = *(
                                    uint32_t *)buffer;
                        }
                    }
                    return read_customer_zone;
                default:
                    return true;
            }
        }
    }
    int already_read = 0;

    bool read_header = false;

    uint32_t id = 0;

    uint32_t request_type = 0;

    bool read_customer_zone = false;

    uint32_t zone_id = 0;


    uint32_t shop_id = 0;

    uint32_t number_of_goods = 0;

    bool reading_array = false;
    std::vector<uint32_t> goods;

    char buffer[8]{};
};

class Order {
public:
    Order(uint32_t shop_id, uint32_t customer_id, std::vector<uint32_t> goods)
            : _shop_id(shop_id), _customer_id(customer_id), _goods(std::move(goods)) { }
    uint32_t _shop_id;
    uint32_t _customer_id;
    std::vector<uint32_t> _goods;
};

class Customer {
public:
    Customer(uint32_t id, uint32_t zone) : _id(id), _zone(zone) { }
    Customer() = default;

    uint32_t _id{};
    uint32_t _zone{};
    std::vector<Order>_orders;
};

class Shop {
public:
    Shop(uint32_t shop_id, uint32_t zone, std::set<uint32_t> goods)
            : _id(shop_id), _zone(zone), _goods(std::move(goods)) { }
    Shop() = default;

    uint32_t _id{};
    uint32_t _zone{};
    std::set<uint32_t> _goods;
    std::vector<Order>_orders;
};

int main(int argc , char *argv[])
{
    if (argc < 2) {
        std::cout << "Usage: app_port" << std::endl;
        exit(1);
    }

    const auto PORT = static_cast<const uint16_t>(atoi(argv[1]));

    int opt = 1;
    int master_socket, addrlen, new_socket, activity, sd;
    int max_sd;
    struct sockaddr_in address;

    std::map<uint32_t, Shop> shop_clients;
    std::map<uint32_t, Customer> customer_clients;
    std::map<int, Request> active_sockets;
    std::set<int> shut_sockets;

    //set of socket descriptors  
    fd_set readfds{};

    //create a master socket  
    if ((master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections ,  
    //this is just a good habit, it will work without this  
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //type of socket created  
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = SOCK_STREAM;
    address.sin_port = htons(PORT);

    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    //try to specify maximum of 3 pending connections for the master socket  
    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection  
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while(true) {
        //clear the socket set  
        FD_ZERO(&readfds);

        //add master socket to set  
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        for (auto const&active_socket : active_sockets) {
            FD_SET(active_socket.first, &readfds);
            if (active_socket.first > max_sd) {
                max_sd = sd;
            }
        }

        //wait for an activity on one of the sockets , timeout is NULL ,  
        //so wait indefinitely  
        activity = select(max_sd + 1, &readfds, nullptr, NULL, NULL);

        if ((activity < 0) && (errno!=EINTR)) {
            printf("select error");
        }

        //If something happened on the master socket ,  
        //then its an incoming connection  
        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs
                    (address.sin_port));

            //send new connection greeting message  
            //if( send(new_socket, message, strlen(message), 0) != strlen(message) ) {
            //    perror("send");
            //}

            active_sockets[new_socket] = Request();
        }

        //else its some IO operation on some other socket 
        for (auto &active_socket : active_sockets) {
            int sd = active_socket.first;
            if (FD_ISSET(sd, &readfds)) {
                if (active_socket.second.try_reading(active_socket.first)) {
                    Request request = active_socket.second;
                    int32_t status = 0;
                            printf("Received new order request on %d goods from client %d for shop %d\n",
                                   request.number_of_goods, request.id, request.shop_id);
                    switch (request.request_type) {
                        case MAKE_ORDER_REQUEST:
                            if (customer_clients.count(request.id) > 0 && shop_clients.count(request.shop_id) > 0) {
                                puts("Added order to customer and shop history");
                                Order new_order = Order(request.shop_id, request.id, request.goods);
                                customer_clients[request.id]._orders.push_back(new_order);
                                shop_clients[request.id]._orders.push_back(new_order);
                            } else {
                                puts("No matching active customer or shop was found");
                                status = 1;
                            }

                            send(sd, &request.request_type, sizeof(request.request_type), 0);
                            send(sd, &status, sizeof(status), 0);
                            break;
                        case GET_ORDER_HISTORY_REQUEST:
                            printf("Received new order history request from client %d\n", request.id);
                            if (customer_clients.count(request.id) + shop_clients.count(request.id) > 0) {
                                std::vector<Order>& orders = customer_clients.count(request.id) > 0
                                                             ? customer_clients[request.id]._orders
                                                             : shop_clients[request.id]._orders;
                                printf("Found %d orders", static_cast<int>(orders.size()));
                                auto number_of_orders = static_cast<uint32_t>(orders.size());

                                send(sd, &request.request_type, sizeof(request.request_type), 0);
                                send(sd, &number_of_orders, sizeof(number_of_orders), 0);
                                for (auto const& order : orders) {
                                    auto number_of_goods = static_cast<uint32_t>(order._goods.size());
                                    send(sd, &(order._customer_id), sizeof(order._customer_id), 0);
                                    send(sd, &(order._shop_id), sizeof(order._shop_id), 0);
                                    send(sd, &number_of_goods, sizeof(number_of_goods), 0);
                                    for (auto const& good : order._goods) {
                                        send(sd, &good, sizeof(good), 0);
                                    }
                                }

                            } else {
                                puts("Unknown client");
                            }
                            break;
                        case ADD_NEW_SHOP_REQUEST:
                            printf("Received add new shop request for client %d in zone %d with %d goods",
                                   request.id, request.zone_id, request.number_of_goods);
                            if (customer_clients.count(request.id) + shop_clients.count(request.id) == 0) {
                                printf("New id is not yet owned by anyone. Creating new shop account");
                                shop_clients[request.id] = Shop(request.id, request.zone_id,
                                                                std::set<uint32_t>(request.goods.begin(),
                                                                                   request.goods.end()));
                            } else {
                                printf("This is is already in use.");
                                status = 1;
                            }

                            send(sd, &request.request_type, sizeof(request.request_type), 0);
                            send(sd, &status, sizeof(status), 0);
                            break;
                        case SHUT_DOWN_CLIENT_REQUEST:
                            printf("Received shutdown request for client %d\n", request.id);
                            if (customer_clients.count(request.id) + shop_clients.count(request.id) > 0) {
                                printf("Shutting down client %d\n", request.id);
                                customer_clients.erase(request.id);
                                shop_clients.erase(request.id);
                                shut_sockets.insert(active_socket.first);
                                close(active_socket.first);
                            } else {
                                puts("Unknown client");
                            }
                            break;
                        case ADD_CUSTOMER_REQUEST:
                            printf("Received add new customer request for client %d in zone %d\n", request.id, request.zone_id);
                            if (customer_clients.count(request.id) + shop_clients.count(request.id) == 0) {
                                printf("New id is not yet owned by anyone. Creating new customer account");
                                customer_clients[request.id] = Customer(request.id, request.zone_id);
                            } else {
                                printf("This id is already in use");
                                status = 1;
                            }
                            send(sd, &request.request_type, sizeof(request.request_type), 0);
                            send(sd, &status, sizeof(status), 0);
                            break;
                        default:
                            printf("Unknown request: %d", request.request_type);
                    }
                    active_socket.second = Request();
                }
            }
        }
        for (auto& socket_ : shut_sockets) {
            active_sockets.erase(socket_);
        }
        shut_sockets.clear();
    }
} 