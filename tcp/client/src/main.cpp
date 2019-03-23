#include <cstdint>
#include <netinet/in.h>
#include <iostream>
#include <netdb.h>
#include <memory.h>
#include <unistd.h>
#include "../include/client.h"

void usage(char *const *argv) { std::cerr << "Usage: " << argv[0] << " <hostname> <port>\n"; }

void help() { std::cerr << "List of commands:\n"
                           "\ta player <name>\n"
                           "\ta croupier <key-phrase>\n"
                           "\tbet <N> (odd|even|<X>)\n"
                           "\tspin\n"
                           "\tfinish\n"
                           "\tlist\n"
                           "\tbye\n"; }

int main(int argc, char *argv[]) {
    if (argc < 3) {
        usage(argv);
        exit(0);
    }

    auto port_number = static_cast<uint16_t>(std::stoi(argv[2]));

    RouletteClient client{};

    if (!client.StartClient(argv[1], port_number))
        exit(1);

    char arg[256];
    int number, sum;
    for (std::string line; client.GetLine(line);) {
        if (sscanf(line.c_str(), "a player %s", arg)) {
            client.AuthorisePlayer(arg);
        } else if (sscanf(line.c_str(), "a croupier %s", arg)) {
            client.AuthoriseCroupier(arg);
        } else if (line ==  "spin") {
            client.StartDraw();
        } else if (line == "finish") {
            client.FinishDraw();
        } else if (line == "list") {
            client.ListBets();
        } else if (line == "bye") {
            client.Quit();
            return 0;
        } else if (sscanf(line.c_str(), "bet %d %d", &sum, &number) == 2 and sum > 0) {
            client.NewBet(std::to_string(number), sum);
        } else if (sscanf(line.c_str(), "bet %d odd", &sum) and sum > 0) {
            client.NewBet("odd", sum);
        } else if (sscanf(line.c_str(), "bet %d even", &sum) and sum > 0) {
            client.NewBet("even", sum);
        } else {
            help();
        }
        client.PrintPrompt();
    }

    return 0;
}
