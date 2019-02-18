#include <cstdint>
#include <netinet/in.h>
#include <iostream>
#include <netdb.h>
#include <memory.h>
#include <unistd.h>
#include "../include/client.h"

void Usage(char* const* argv) { std::cerr << "Usage: " << argv[0] << " <hostname> <port>\n"; }

void AuthHelp() {
    std::cout << "Please authorize as a customer or as a freelancer:\n"
                 "\ta freelancer <name>\n"
                 "\ta customer <name>\n";
}

void FreelancerHelp() {
    std::cout << "List of commands:\n"
                 "\t orders\n"
                 "\t request <order_id>\n"
                 "\t start <order_id>\n"
                 "\t finish <order_id>\n"
                 "\t bye\n";
}

void CustomerHelp() {
    std::cout << "List of commands\n"
                 "\t new order <description>\n"
                 "\t all orders\n"
                 "\t my orders\n"
                 "\t give <order_id>\n"
                 "\t done <order_id>\n"
                 "\t bye\n";
}

void WorkWithFreelancer(MarketClient& client) {
    std::string line;
    int order_id;

    FreelancerHelp();
    client.PrintPrompt();

    while (true) {
        client.GetLine(line);
        if (line == "orders") {
            client.ListOpenOrders();
        } else if (sscanf(line.c_str(), "request %d", &order_id)) {
            client.RequestOrder(order_id);
        } else if (sscanf(line.c_str(), "start %d", &order_id)) {
            client.StartOrder(order_id);
        } else if (sscanf(line.c_str(), "start %d", &order_id)) {
            client.FinishOrder(order_id);
        } else if (line == "bye") {
            client.Quit();
            return;
        } else {
            FreelancerHelp();
        }
        client.PrintPrompt();
    }
}

void WorkWithCustomer(MarketClient& client) {
    char arg[256];
    std::string line;
    int order_id;

    CustomerHelp();
    client.PrintPrompt();
    while (true) {
        client.GetLine(line);
        if (line == "all orders") {
            client.ListOpenOrders();
        } else if (line == "my orders") {
            client.ListMyOrders();
        } else if (sscanf(line.c_str(), "give %d", &order_id)) {
            client.GiveOrder(order_id);
        } else if (sscanf(line.c_str(), "done %d", &order_id)) {
            client.ApproveDoneOrder(order_id);
        } else if (sscanf(line.c_str(), "new order %s", arg)) {
            client.NewOrder(arg);
        } else if (line == "bye") {
            client.Quit();
            return;
        } else {
            CustomerHelp();
        }
        client.PrintPrompt();
    }

}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        Usage(argv);
        exit(0);
    }

    auto port_number = static_cast<uint16_t>(std::stoi(argv[2]));

    MarketClient client{};

    if (!client.StartClient(argv[1], port_number))
        exit(1);

    char arg[256];
    std::string line;

    AuthHelp();

    while (true) {
        client.GetLine(line);
        if (sscanf(line.c_str(), "a customer %s", arg)) {
            if (client.AuthoriseCustomer(arg)) {
                WorkWithCustomer(client);
                break;
            }
        } else if (sscanf(line.c_str(), "a freelancer %s", arg)) {
            if (client.AuthoriseFreelancer(arg)) {
                WorkWithFreelancer(client);
                break;
            }
        } else {
            AuthHelp();
        }
    }

    return 0;
}
