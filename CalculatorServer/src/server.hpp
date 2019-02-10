#include "operations.hpp"
#include "request_parser.hpp"
#include <memory>
#include <future>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <set>

using namespace operations;

struct Server {
    enum Status {
        Ok = 0,
        Fail = -1
    };

    void sendResponse(int socket, operations::Type type, int32_t value) {
        int responseSize = 4 + 1 + 1;
        char *buffer = new char[responseSize];
        for (int i = 0; i < 4; ++i) {
            buffer[i] = static_cast<uint8_t>(value & 0xffu);
            value >>= 8;
        }
        buffer[4] = type;
        buffer[5] = Ok;
        sendMessage(socket, buffer, responseSize);

    }

    void sendError(int socket) {
        char *buffer = new char[1];
        buffer[0] = Fail;
        sendMessage(socket, buffer, 1);
    }


    void execute() {
        int clientSocket;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);


        int sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0) {
            perror("socket failed");
            exit(1);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(sockfd, (struct sockaddr *) &address, sizeof(address)) < 0) {
            perror("ERROR on binding");
            exit(1);
        }

        listen(sockfd, 5);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
        while (true) {
            if ((clientSocket = accept(sockfd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
                perror("accept");
                continue;
            }
            sockets.insert(clientSocket);

            RequestHandler handler(clientSocket, std::shared_ptr<Server>(this));
            std::thread thr(handler);
            thr.detach();
        }
#pragma clang diagnostic pop
    }

private:
    std::set<int> sockets;
    const uint16_t port = 1000;

    void sendMessage(int socket, char *buffer, size_t size) {
        ssize_t i = send(socket, buffer, size, 0);
        if (i < 0) {
            perror("Whoops :c");
        }
    }

    struct RequestHandler {
        RequestHandler(int socket, const std::shared_ptr<Server> &server) : socket(socket), server(server) {}

        void operator()() {
            try {
                std::shared_ptr<Operation> operation(parseRequest(socket));
                auto callback = [&]() {
                    int32_t result = operation.get()->evaluate();
                    server.get()->sendResponse(socket, operation.get()->getType(), result);
                };
                std::async(std::launch::async, callback);
            } catch (const std::exception &e) {

            }
        };


    private:
        int socket;
        std::shared_ptr<Server> server;
    };
};

