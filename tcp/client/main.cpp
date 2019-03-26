#include <iostream>
#include <string>
#include <vector>
#include "client.h"

void process_incoming_event(client *client1) {
    std::string incoming_event = client1->receive_incoming_event();
    if (incoming_event.empty()) {
        std::cout << "Server has disconnected. Bye!" << std::endl;
        exit(0);
    }
    std::cout << client1->receive_incoming_event();
}

int main(int argc, char * argv []) {
    if (argc < 3) {
        std::cout << "Usage: hostname port" << std::endl;
        exit(1);
    }

    std::cout << "Please enter your client ID: ";
    int client_id;
    std::cin >> client_id;

    client * client1 = new client(argv[0], atoi(argv[1]), client_id);

    std::cout << "Connected to the food server" << std::endl;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true) {
        std::cout << "Please choose an action:" << std::endl;
        std::cout << "[c]reate order" << std::endl;
        std::cout << "[r]etrieve order history" << std::endl;
        std::cout << "[a]dd a new shop" << std::endl;
        std::cout << "[d]isconnect" << std::endl;

        char input;
        std::cin >> input;

        switch (input) {
            case 'c': {
                std::cout << "Creating an order" << std::endl;
                std::cout << "Please enter information in the following format: "
                             "shop_id num_goods goods_ids{num_goods}" << std::endl;
                int shop_id;
                int num_goods;
                std::cin >> shop_id >> num_goods;
                std::vector<int> goods_ids(num_goods);
                for (int &good_id : goods_ids) {
                    std::cin >> good_id;
                }
                client1->create_order(shop_id, goods_ids);
                process_incoming_event(client1);
                break;
            }
            case 'r': {
                std::cout << "Retrieving an order" << std::endl;
                client1->retrieve_order_history();
                process_incoming_event(client1);
                break;
            }
            case 'a': {
                std::cout << "Adding a new shop" << std::endl;
                std::cout << "Please enter information in the following format:"
                             "num_goods goods_ids{num_goods} zone_id" << std::endl;
                int num_goods;
                std::cin >> num_goods;
                std::vector<int> goods_ids(num_goods);
                for (int & good_id : goods_ids) {
                    std::cin >> good_id;
                }
                int zone_id;
                std::cin >> zone_id;
                client1->add_new_shop(goods_ids, zone_id);
                process_incoming_event(client1);
                break;
            }
            case 'd': {
                std::cout << "Disconnecting. Bye!" << std::endl;
                client1->disconnect();
                delete client1;
                exit(0);
                break;
            }
            default: {
                std::cout << "Incorrect input: " << input << std::endl;
                break;
            }
        }


    }
#pragma clang diagnostic pop

    return 0;
}