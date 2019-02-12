//
// Created by Владислав Калинин on 11/02/2019.
//

#include <cstdio>
#include <cstdlib>
#include <set>
#include <sys/select.h>
#include <arpa/inet.h>
#include <zconf.h>
#include <string>
#include <iostream>
#include <netdb.h>
#include "../include/ChatServer.h"
#include "../include/Tasks.h"

ChatServer::ChatServer(const char *hostname, uint16_t port) : thread_pool(4), online_users() {
    int opt = true;

    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt,
                   sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    hostent *server = gethostbyname(hostname);
    bcopy(server->h_addr, (char *) &address.sin_addr.s_addr, (size_t) server->h_length);
    address.sin_port = htons(port);

    if (bind(master_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", port);

    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

void ChatServer::run() {
    fd_set readfds;
    std::set<int> clients;
    clients.clear();
    int addrlen = sizeof(address);
    int new_socket, valread;
    char buffer[1025];

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);

        for (auto it{clients.begin()}, end{clients.end()}; it != end; it++) {
            FD_SET(*it, &readfds);
        }

        int mx = std::max(master_socket, *max_element(clients.begin(), clients.end()));
        if (select(mx + 1, &readfds, NULL, NULL, NULL) <= 0) {
            perror("select");
            exit(3);
        }

        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket,
                                     (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n", new_socket,
                   inet_ntoa(address.sin_addr), ntohs
                   (address.sin_port));
            clients.insert(new_socket);
        }

        for (auto it{clients.begin()}, end{clients.end()}; it != end;) {
            if (online_users.find(*it) != online_users.end()) {
                User user = online_users.at(*it);
                if (user.was_kicked) {
                    close(*it);
                    online_users.unsafe_erase(*it);
                    it = clients.erase(it);
                    continue;
                }
            }
            if (FD_ISSET(*it, &readfds)) {
                try {
                    if ((valread = read(*it, buffer, 1024)) == 0) {
                        getpeername(*it, (struct sockaddr *) &address, (socklen_t *) &addrlen);
                        printf("Host disconnected , ip %s , port %d \n",
                               inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                        close(*it);
                        online_users.unsafe_erase(*it);
                        it = clients.erase(it);
                    } else {
                        buffer[valread] = '\0';
                        try {
                            json request = json::parse(buffer);
                            ServerTask *task = ServerTask::create(request, &(this->online_users), *it);
                            if (task != nullptr) {
                                thread_pool.push(std::ref(*task));
                            }
                        } catch (detail::parse_error e) {
                            printf("parse error\n");
                        }
                        it++;
                    }
                } catch (std::exception e) {
                    std::cout << e.what() << std::endl;
                    break;
                }
            } else {
                it++;
            }
        }
    }
}