#ifndef NETWORKS_BUG_TRACKING_SERVER_H
#define NETWORKS_BUG_TRACKING_SERVER_H

#include <cstdint>
#include <map>
#include "UserService.h"
#include "Bug.h"

class BugTrackingServer {
private:
    class Client {
    public:
        const int sock_fd;
        UserService::User user;

        explicit Client(int socket_fd, UserService* userService);

        bool isAuthorized();

        bool authorize(int id);
    private:
        UserService* _userService;
    };

public:
    explicit BugTrackingServer(UserService* userService);

    void initServer(uint16_t port_number);

    bool authorize(Client& client);

    bool bugsTesterList(Client& client);

    bool bugVerification(Client& client);

    bool bugsDeveloperList(Client& client);

    bool bugFix(Client& client);

    bool bugRegister(Client& client);

    bool close(Client& client);

private:
    UserService* _userService;
    std::map<int, Client*> _clients;
    std::mutex _clients_mutex;

    std::map<int, Bug> _bugs;
    std::mutex _bugs_mutex;

    void process_client(int sock_fd);

    static bool writeInt32(int sock_fd, uint32_t number);

    static bool writeString(int sock_fd, std::string data);

    static bool readInt32(int sock_fd, uint32_t& dst);

    static bool readString(int sock_fd, std::string& dst);
};


#endif