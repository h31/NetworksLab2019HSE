#ifndef NETWORKS_LABS_SERVER_H
#define NETWORKS_LABS_SERVER_H

#include <tbb/concurrent_unordered_map.h>
#include <model/request.h>
#include <model/response.h>
#include <lib/ctpl.h>
#include "EmailWithInfo.h"

class Server {
private:
    ctpl::thread_pool thread_pool;
    uint16_t port;
    std::atomic_uint32_t idCounter;
    tbb::concurrent_unordered_map<uint32_t, EmailWithInfo> emails;
public:
    explicit Server(uint16_t port);

    void run_server();

private:
    class Worker : request::RequestVisitor {
    private:
        int socket;
        Server &server;

    public:
        explicit Worker(int socket, Server &server);

        void operator() (int id);

    private:
        uint32_t readMessageSize();
        void readToBuffer(uint8_t * buffer, size_t size);

        void visitSendEmailRequest(const request::SendEmailRequest *request) override;

        void visitCheckEmailRequest(const request::CheckEmailRequest *request) override;

        void visitGetEmailRequest(const request::GetEmailRequest *request) override;

        void sendResponse(std::unique_ptr<response::Response> &&response);
    };
};

#endif //NETWORKS_LABS_SERVER_H
