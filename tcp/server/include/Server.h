#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <unordered_map>
#include <mutex>

#include "serverMessages/GroupMessageMessage.h"
#include "serverMessages/MessageMessage.h"

class Client;

class Server {

private:
    int server_socket_fd;
    std::unordered_map<std::string, Client *> active_clients;
    std::mutex active_clients_mtx;

    void ShutdownSocket();

public:
    explicit Server(unsigned short port);
    ~Server();
    Server(const Server &) = delete;
    Server(Server &&) = delete;
    Server &operator=(const Server &) = delete;
    Server &operator=(Server &&) = delete;

    int Accept();
    void RemoveClient(const std::string &user_name);
    bool AddClient(const std::string &user_name, Client *client);
    void Run();
    void SendToAll(const std::string &from, const GroupMessageMessage &message);
    bool SendTo(const std::string &from, const std::string &to, const MessageMessage &message);
    void ShutdownAllClients();
};

#endif
