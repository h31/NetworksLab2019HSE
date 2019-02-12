#ifndef NETWORKS_LABS_SERVER_H
#define NETWORKS_LABS_SERVER_H

#include <tbb/concurrent_unordered_map.h>
#include <model/request.h>
#include <model/response.h>
#include <atomic>
#include <thread>
#include <set>
#include "EmailWithId.h"

class Server {
private:
    uint16_t port;
    std::atomic_uint32_t clientIdCounter;
    std::atomic_uint32_t idCounter;
    tbb::concurrent_unordered_map<uint32_t, EmailWithId> emails;
    std::set<int> sockets;
    std::atomic_bool onShutdown;
public:
    explicit Server(uint16_t port);

    void runServer();

    void shutdown();

private:
    class Worker : request::RequestVisitor {
    private:
        int socket;
        uint32_t clientId;
        Server &server;

    public:
        Worker(int socket, uint32_t clientId, Server &server);

        void operator() ();

    private:
        void visitSendEmailRequest(const request::SendEmailRequest *request) override;

        void visitCheckEmailRequest(const request::CheckEmailRequest *request) override;

        void visitGetEmailRequest(const request::GetEmailRequest *request) override;
    };
};

#endif //NETWORKS_LABS_SERVER_H
