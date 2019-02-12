#include <stdio.h>
#include <stdlib.h>

#include "../../common/include/json.hpp"
#include "../include/ChatClient.h"
#include "../include/utils.h"
#include "../../common/include/ctpl.h"
#include <iostream>

using namespace nlohmann;

std::string register_format = "\tregister format: \\register <username> <password>";
std::string login_format = "\tlogin format: \\login <username> <password>";

void authorization(ChatClient &client);

std::string getUserName(std::string msg);

void receiveResponses(int id, ChatClient &client);

int main(int argc, char *argv[]) {

    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    ChatClient client(argv[1], atoi(argv[2]));
    authorization(client);

    ctpl::thread_pool thread_pool(1);
    thread_pool.push(receiveResponses, client);
    std::string msg;

    try {
        while (true) {
            getline(std::cin, msg);
            if (msg == "") {
                continue;
            }
            if (msg == "\\exit") {
                break;
            }
            if (msg == "\\kick") {
                std::cout << "please enter username: ";
                std::string name;
                std::cin >> name;
                client.kickUser(name);
                continue;
            }
            std::string username = getUserName(msg);
            if (username == "") {
                client.sendBroadcastMessage(msg);
            } else {
                client.sendPrivateMessage(username, msg);
            }
        }
    } catch (std::exception e) {
        std::cout << e.what() << std::endl;
    }

    client.close_client();

    return 0;
}

void receiveResponses(int id, ChatClient &client) {
    while (true) {
        TextMessage *message = client.recieveMessage();
        std::cout << message->getSender() << ": " << message->getMessage() << std::endl;
        delete message;
    }
}

std::string getUserName(std::string msg) {
    std::string username = firstWord(msg);
    if (username[0] == '@') {
        return username.substr(1);
    }
    return "";
}

void authorization(ChatClient &client) {
    std::cout << "please register or enter username and password:" << std::endl;
    std::cout << register_format << std::endl;
    std::cout << login_format << std::endl;

    std::string cmd_line;
    std::string param;
    std::vector<std::string> params;

    while (true) {
        getline(std::cin, cmd_line);
        split(cmd_line, params, ' ');
        if (params[0] == "\\register") {
            if (params.size() != 3) {
                std::cout << register_format << std::endl;
            } else {
                try {
                    client.registry(params[1], params[2]);
                    std::cout << "registration successfull" << std::endl;
                } catch (std::runtime_error e) {
                    std::cout << e.what() << std::endl;
                } catch (std::exception e) {
                    std::cout << e.what() << std::endl;
                }
            }
        } else if (params[0] == "\\login") {
            if (params.size() != 3) {
                std::cout << login_format << std::endl;
            } else {
                try {
                    client.login(params[1], params[2]);
                    std::cout << "login successfull" << std::endl;
                    break;
                } catch (std::runtime_error e) {
                    std::cout << e.what() << std::endl;
                } catch (std::exception e) {
                    std::cout << e.what() << std::endl;
                }
            }
        } else {
            std::cout << "unknow command" << std::endl;
        }
        params.clear();
    }
}