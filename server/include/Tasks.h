//
// Created by Владислав Калинин on 11/02/2019.
//

#ifndef NETWORKS_TASKS_H
#define NETWORKS_TASKS_H

#include "../../common/include/json.hpp"
#include "../../common/include/constants.h"
#include "ChatServer.h"

using namespace nlohmann;

class ServerTask {

public:
    virtual void operator()(int id) = 0;

    static ServerTask *create(const json &request, tbb::concurrent_unordered_map<int, User> *online_users_, int fd);

protected:
    ServerTask(int fd_) : fd(fd_) {}

    virtual void sendOkResponse();

    virtual void sendErrorResponse(std::string msg);

    virtual void sendResponce(int socketfd, std::string msg);

protected:
    int fd;
};

class RegistryTask : public ServerTask {
public:
    explicit RegistryTask(const json &request, int fd_) : ServerTask(fd_), name(request.at(NAME).get<std::string>()),
                                                          password(request.at(PASSWORD).get<std::string>()) {}

    void operator()(int id) override;

private:
    std::string name;
    std::string password;
};

class AuthorizationTask : public ServerTask {
public:
    AuthorizationTask(const json &request, tbb::concurrent_unordered_map<int, User> *online_users_, int fd_) :
            ServerTask(fd_), name(request.at(NAME).get<std::string>()),
            password(request.at(PASSWORD).get<std::string>()), online_users(online_users_) {}

    void operator()(int id) override;

private:
    std::string name;
    std::string password;
    tbb::concurrent_unordered_map<int, User> *online_users;
};

class BroadcastTask : public ServerTask {
public:
    BroadcastTask(const json &request, tbb::concurrent_unordered_map<int, User> online_users_, int fd_) :
            ServerTask(fd_), msg(request.dump()), online_users(online_users_) {}

    void operator()(int id) override;

private:
    std::string msg;
    tbb::concurrent_unordered_map<int, User> online_users;
};

class PrivateTask : public ServerTask {
public:
    PrivateTask(const json &request, tbb::concurrent_unordered_map<int, User> online_users_, int fd_) :
            ServerTask(fd_), msg(request.dump()), receiver(request.at(RECEIVER).get<std::string>()),
            online_users(online_users_) {}

    void operator()(int id) override;

private:
    std::string msg;
    std::string receiver;
    tbb::concurrent_unordered_map<int, User> online_users;
};

class KickTask : public ServerTask {
public:
    KickTask(const json &request, tbb::concurrent_unordered_map<int, User> *online_users_, int fd_) :
            ServerTask(fd_), name(request.at(NAME).get<std::string>()),
            online_users(online_users_) {}

    void operator()(int id) override;

private:
    std::string name;
    tbb::concurrent_unordered_map<int, User> *online_users;
};

#endif //NETWORKS_TASKS_H
