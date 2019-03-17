#include <netinet/in.h>
#include <netdb.h>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <string>
#include <memory>
#include "operations.hpp"
#include "protocol.hpp"
#include <sys/socket.h>
#include <iostream>

using std::string;


using namespace operations;

struct CalcClient {


    CalcClient(const char *hostname, uint16_t port) {
        sockaddr_in serv_addr;
        hostent *server;

        sock_desc = socket(AF_INET, SOCK_STREAM, 0);

        if (sock_desc < 0) {
            perror("ERROR opening socket");
            exit(1);
        }

        server = gethostbyname(hostname);

        if (server == nullptr) {
            fprintf(stderr, "ERROR, no such host\n");
            exit(0);
        }

        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
        serv_addr.sin_port = htons(port);

        if (connect(sock_desc, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("ERROR connecting");
            exit(1);
        }
    }

    void execute(const string &type, int32_t arg1, int32_t arg2) {
        Type req = operations::typeFromString(type);
        protocol::Response *response = doRequest(req, arg1, arg2);

        if (response->getStatus() == protocol::status::Ok) {
            std::cout << "Evaluation result: " << (static_cast<protocol::Ok *>(response))->getResult();
        } else {
            std::cout << "Wrong result :c" << std::endl;
        }

        delete response;
    }

private:
    protocol::Response *doRequest(operations::Type type, int32_t arg1, int32_t arg2) {
        char *buffer = new char[protocol::REQUEST_SIZE];
        switch (type) {
            case Fact:
            case Sq: {
                protocol::LongOperationRequest(arg1, arg2, type).toCharArray(buffer);
            };

            case Add:
            case Subs:
            case Div:
            case Mult: {
                protocol::QuickOperationRequest(arg1, arg2, type).toCharArray(buffer);
            }
                ssize_t i = -1;
                while (i < protocol::REQUEST_SIZE) {
                    i = send(sock_desc, buffer + i, protocol::REQUEST_SIZE - i, 0);
                }
                return protocol::parseResult(sock_desc);
        }
    }


private:
    int sock_desc;
};