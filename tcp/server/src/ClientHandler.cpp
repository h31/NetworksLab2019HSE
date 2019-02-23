#include "ClientHandler.h"

ClientHandler::ClientHandler(std::shared_mutex *mutex, int sockfd,
                             std::unordered_map<uint32_t, struct rating> *ratings, uint32_t* freeId) {
    this->mutex = mutex;
    this->sockfd = sockfd;
    this->ratings = ratings;
    this->freeId = freeId;
}

ReadWriteHelper ClientHandler::readWriteHelper;

void ClientHandler::run() {
    while(getRequest());
}

bool ClientHandler::getRequest(){
    ssize_t n;
    bzero(buffer, 4);
    n = read(sockfd, buffer, 4);

    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    uint32_t message_type = readWriteHelper.get4Bytes(buffer, 0);

    switch(message_type) {
        case 0:
            processGetRatingsRequest();
            break;
        case 1:
            processGetRatingRequest();
            break;
        case 2:
            processAddRatingRequest();
            break;
        case 3:
            processRemoveRatingRequest();
            break;
        case 4:
            processAddRatingOption();
            break;
        case 5:
            processOpenCloseRequest();
            break;
        case 6:
            processVoteRequest();
            break;
        default:
            processWrongRequest();
    }
    return true;
}

void* ClientHandler::staticFunction(void* p){
    static_cast<ClientHandler*>(p)->run();
    return nullptr;
}

void ClientHandler::processGetRatingRequest() {
    readFromSocket(4);
    uint32_t id = readWriteHelper.get4Bytes(buffer, 4);
    size_t emptyLen = 1024;
    bzero(buffer, 37);
    emptyLen -= 37;
    mutex->lock();
    struct rating r = (*ratings)[id];
    mutex->unlock();
    int offset = 0;
    for (char &i : r.name) {
        readWriteHelper.setByte(buffer, offset, static_cast<uint32_t>(i));
        offset++;
    }
    readWriteHelper.setByte(buffer, offset, r.isOpen ? 1 : 0);
    offset++;
    readWriteHelper.set4Bytes(buffer, offset, r.options.size());
    writeToSocket(37);
    offset = 0;
    bzero(buffer, 36 * r.options.size());
    emptyLen -= 36 * r.options.size();
    for (int i = 0; i < r.options.size(); i++) {
        struct option o = r.options[i];
        readWriteHelper.set4Bytes(buffer, offset, i);
        offset += 4;
        for (char &j : o.name) {
            readWriteHelper.setByte(buffer, offset, static_cast<uint32_t>(j));
            offset++;
        }
        for (int j = 0 ; j < 32 - o.name.length(); j++) {
            readWriteHelper.setByte(buffer, offset, 0);
            offset++;
        }
        readWriteHelper.set4Bytes(buffer, offset, o.voteNumber);
        offset += 4;
    }
    writeToSocket(36 * r.options.size());
    bzero(buffer, emptyLen);
    writeToSocket(emptyLen);
}

void ClientHandler::readFromSocket(size_t size) {
    bzero(buffer, size);
    if (read(sockfd, buffer, size)) {
        perror("ERROR reading from socket");
        exit(1);
    }
}

void ClientHandler::writeToSocket(size_t size) {
    if (write(sockfd, buffer, size)) {
        perror("ERROR writing to socket");
        exit(1);
    }
}

void ClientHandler::processAddRatingRequest() {
    readFromSocket(36);
    size_t emptyLen = 1024 - 36 - 4;
    std::string name;
    int offset = 0;
    for (int i = 0; i < 32; i++) {
        name += readWriteHelper.getByte(buffer, offset);
        offset++;
    }
    uint32_t number = readWriteHelper.get4Bytes(buffer, offset);
    readFromSocket(number * 32);
    emptyLen -= number * 32;
    offset = 0;
    std::vector<struct option> options;
    for (int i = 0; i < number; i++) {
        std::string optionName;
        for (int j = 0; j < 32; j++) {
            optionName += readWriteHelper.getByte(buffer, offset);
            offset++;
        }
        struct option o;
        o.name = optionName;
        options.push_back(o);
    }
    struct rating r;
    r.name = name;
    r.isOpen = true;
    r.options = options;

    mutex->lock();
    r.id = *freeId;
    (*freeId)++;
    (*ratings)[r.id] = r;
    mutex->unlock();
    readFromSocket(emptyLen);
    bzero(buffer, 1);
    readWriteHelper.set4Bytes(buffer, 0, 1);
    writeToSocket(1);
    bzero(buffer, 255);
    writeToSocket(255);
}

void ClientHandler::processRemoveRatingRequest() {
    readFromSocket(4);
    uint32_t id = readWriteHelper.get4Bytes(buffer, 4);
    mutex->lock();
    (*ratings).erase(id);
    mutex->unlock();
    readFromSocket(256 - 8);
    bzero(buffer, 1);
    readWriteHelper.set4Bytes(buffer, 0, 1);
    writeToSocket(1);
    bzero(buffer, 255);
    writeToSocket(255);
}

void ClientHandler::processAddRatingOption() {
    readFromSocket(4);
    uint32_t id = readWriteHelper.get4Bytes(buffer, 4);
    size_t emptyLen = 256 - 8;
    bzero(buffer, 32);
    readFromSocket(32);
    std::string optionName;
    int offset = 0;
    for (int j = 0; j < 32; j++) {
        optionName += readWriteHelper.getByte(buffer, offset);
        offset++;
    }
    struct option o;
    o.name = optionName;
    o.voteNumber = 0;
    (*ratings)[id].options.push_back(o);
    emptyLen -= 32;
    readFromSocket(emptyLen);
    bzero(buffer, 1);
    readWriteHelper.set4Bytes(buffer, 0, 1);
    writeToSocket(1);
    bzero(buffer, 255);
    writeToSocket(255);
}

void ClientHandler::processOpenCloseRequest() {
    readFromSocket(256);
    writeToSocket(256);
}

void ClientHandler::processVoteRequest() {
    readFromSocket(256);
    writeToSocket(256);
}

void ClientHandler::processWrongRequest() {
}

void ClientHandler::processGetRatingsRequest() {
    readFromSocket(256 - 4);
    writeToSocket(1024);
}
