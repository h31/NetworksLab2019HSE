#pragma once

#include "server.h"
#include <shared_mutex>
#include <cstdint>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include "ReadWriteHelper.h"
#include <iostream>

struct option {
    std::string name;
    uint32_t voteNumber = 0;
};

struct rating {
    uint32_t id;
    std::string name;
    bool isOpen = true;
    std::vector<struct option> options;
};

class ClientHandler {
  public:
    ClientHandler(std::shared_mutex *mutex, int sockFd,
                  std::unordered_map<uint32_t, struct rating> *ratings, uint32_t * freeId);
    void run();
    static void* staticFunction(void* p);


private:
    std::shared_mutex* mutex;
    uint8_t buffer[256];
    int sockfd;
    bool getRequest();
    static ReadWriteHelper readWriteHelper;

    void processGetRatingRequest();
    void readFromSocket(size_t size);
    void writeToSocket(size_t size);
    std::unordered_map<uint32_t, struct rating> *ratings;
    uint32_t * freeId;

    void processAddRatingRequest();

    void processRemoveRatingRequest();

    void processAddRatingOption();

    void processOpenCloseRequest();

    void processVoteRequest();

    void processWrongRequest();

    void processGetRatingsRequest();
};
