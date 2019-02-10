#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include <unistd.h>
#include <string>
#include <thread>

class Client {

private:
    int socket_fd;
    std::string user_name;
    std::thread *thread;

public:
    explicit Client(int socket_fd);
    Client(const Client &) = delete;
    Client(Client &&) = default;
    Client &operator=(const Client &) = delete;
    Client &operator=(Client &&) = default;
    ~Client();

    int GetSocket() const;
    bool IsLoggedIn();
    void SetThread(std::thread *thread);
    const std::string &GetUserName();
    void SetUserName(std::string &user_name);
    void Join();
    void ShutdownSocket();
};

#endif
