//
// Created by Olga Alehina on 2019-02-18.
//
#include <Client.h>

#include "Client.h"

void Client::start(char *host, uint16_t port) {
    connect(host, port);
    uint32_t messageType;
    while (true) {
        printHelp();
        std::cin >> messageType;
        switch (messageType) {
            case 0: {
                respond(256, messageType, nullptr);
                printAllTopics();
                break;
            }
            case 1: {
                respond(256, messageType, &voicesCount);
                printTop();
                break;
            }
            case 2: {
                respond(1024, messageType, &createTopic);
                printDefaultResponse();
                break;
            }
            case 3: {
                respond(256, messageType, &removeTopic);
                printDefaultResponse();
                break;
            }
            case 4: {
                respond(256, messageType, &createAlternative);
                printDefaultResponse();
                break;
            }
            case 5: {
                respond(256, messageType, &closeTopic);
                printDefaultResponse();
                break;
            }
            case 6: {
                respond(256, messageType, &vote);
                printDefaultResponse();
                break;
            }
            case 7: {
                return;
            }
            default:
                break;
        }
    }
}

void Client::printHelp() {
    std::cout << "0: получить список тем" << std::endl;
    std::cout << "1: получить количество голосов" << std::endl;
    std::cout << "2: создать тему рейтинга" << std::endl;
    std::cout << "3: удалить тему рейтинга" << std::endl;
    std::cout << "4: добавить новый вариант выбора в рейтинг" << std::endl;
    std::cout << "5: изменить статус рейтинга" << std::endl;
    std::cout << "6: проголосовать" << std::endl;
    std::cout << "7: выйти" << std::endl;
    std::cout << "введите тип сообщения: ";
}

void Client::connect(char *host, uint16_t port) {
    struct sockaddr_in serv_addr;
    struct hostent *server;
    socketFd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    server = gethostbyname(host);

    if (server == nullptr) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(port);

    if (::connect(socketFd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }
}

uint8_t *Client::respond(size_t size, uint32_t type, void (*addParams)(uint8_t *, int &)) {
    auto *buffer = new uint8_t[size];
    bzero(buffer, size);
    int writeOffset = 0;
    addType(buffer, writeOffset, type);
    if (addParams != nullptr) {
        addParams(buffer, writeOffset);
    }
    sendMessage(buffer, 256);
    return buffer;
}


void Client::voicesCount(uint8_t *buffer, int &writeOffset) {
    addTopicId(buffer, writeOffset);
}

void Client::createTopic(uint8_t *buffer, int &writeOffset) {
    addTopicName(buffer, writeOffset);
    addAlternatives(buffer, writeOffset, 1024);
}

void Client::removeTopic(uint8_t *buffer, int &writeOffset) {
    addTopicId(buffer, writeOffset);
}

void Client::createAlternative(uint8_t *buffer, int &writeOffset) {
    addTopicId(buffer, writeOffset);
    addAlternative(buffer, writeOffset);
}

void Client::closeTopic(uint8_t *buffer, int &writeOffset) {
    addTopicId(buffer, writeOffset);
    addTopicStatus(buffer, writeOffset);
}

void Client::vote(uint8_t *buffer, int &writeOffset) {
    addTopicId(buffer, writeOffset);
    addAlternativeId(buffer, writeOffset);
}

void Client::printDefaultResponse() {
    uint8_t responseBuffer[256];
    bzero(responseBuffer, 256);
    ssize_t n = read(socketFd, responseBuffer, 256);
    if (n < 0) {
        std::cout << "ERROR reading to socket" << std::endl;
        perror("ERROR writing to socket");
        return;
    }
    int readOffset = 0;
    uint8_t isSuccess = ReadWriteHelper::getByte(responseBuffer, readOffset);
    if (isSuccess) {
        std::cout << "ОК";
    } else {
        std::cout << "Произошла ошибка";
    }
}


void Client::addTopicId(uint8_t *buffer, int &writeOffset) {
    std::cout << "Введите id темы: ";
    uint32_t topicsId;
    std::cin >> topicsId;
    ReadWriteHelper::set4Bytes(buffer, writeOffset, topicsId);
}


void Client::addAlternativeId(uint8_t *buffer, int &writeOffset) {
    std::cout << "Введите id варианты: ";
    uint32_t alternativeId;
    std::cin >> alternativeId;
    ReadWriteHelper::set4Bytes(buffer, writeOffset, alternativeId);
}


void Client::addType(uint8_t *buffer, int &writeOffset, uint32_t type) {
    ReadWriteHelper::set4Bytes(buffer, writeOffset, type);
}

void Client::sendMessage(uint8_t *buffer, size_t size) {
    ssize_t n = write(socketFd, buffer, size);
    if (n < 0) {
        std::cout << "ERROR writing to socket" << std::endl;
        perror("ERROR writing to socket");
        return;
    }
}

void Client::addTopicName(uint8_t *buffer, int &writeOffset) {
    std::string topicsName;
    std::cout << "Введите название темы: ";
    std::cin >> topicsName;
    ReadWriteHelper::set32Byte(buffer, writeOffset, topicsName);
}

void Client::printAllTopics() {
    uint8_t buffer[1024];
    bzero(buffer, 1024);
    int readOffset = 0;
    ssize_t n = read(socketFd, buffer, 1024);

    if (n < 0) {
        std::cout << "ERROR reading to socket" << std::endl;
        perror("ERROR reading to socket");
        return;
    }
    uint32_t countTopics = ReadWriteHelper::get4Bytes(buffer, readOffset);

    for (int i = 0; i < countTopics; i++) {
        std::string topic = ReadWriteHelper::get32Byte(buffer, readOffset);
        std::cout << topic << std::endl;
    }
}

void Client::printTop() {
    int readOffset = 0;
    uint8_t responseBuffer[1024];
    bzero(responseBuffer, 1024);
    ssize_t n = read(socketFd, responseBuffer, 1024);

    if (n < 0) {
        std::cout << "ERROR reading to socket" << std::endl;
        perror("ERROR reading to socket");
        return;
    }
    std::string topicName = ReadWriteHelper::get32Byte(responseBuffer, readOffset);
    uint32_t countAlternative = ReadWriteHelper::get4Bytes(responseBuffer, readOffset);
    std::cout << "Название темы: " << topicName << std::endl;
    for (int i = 0; i < countAlternative; i++) {
        std::string alternativeName = ReadWriteHelper::get32Byte(responseBuffer, readOffset);
        uint32_t countVoices = ReadWriteHelper::get4Bytes(responseBuffer, readOffset);
        std::cout << alternativeName << ": " << countVoices << std::endl;
    }
}

void Client::addAlternatives(uint8_t *buffer, int &writeOffset, int maxSize) {
    uint32_t alternativeCount = 0;
    std::vector<std::string> alternatives;

    std::cout << "Введите кол-во вариантов: ";
    std::cin >> alternativeCount;

    std::cout << "Введите варианты: ";
    for (int i = 0; i < alternativeCount; i++) {
        std::string alternative;
        std::cin >> alternative;
        alternatives.push_back(alternative);
    }
    ReadWriteHelper::set4Bytes(buffer, writeOffset, alternativeCount);
    for (const std::string &alternative: alternatives) {
        if (writeOffset >= maxSize) break;
        ReadWriteHelper::set32Byte(buffer, writeOffset, alternative);
    }
}


void Client::addAlternative(uint8_t *buffer, int &writeOffset) {
    std::cout << "Введите вариант: ";
    std::string alternative;
    std::cin >> alternative;
    ReadWriteHelper::set32Byte(buffer, writeOffset, alternative);
}

void Client::addTopicStatus(uint8_t *buffer, int &writeOffset) {
    std::cout << "Введите 1, если хотите закрыть тему голосования, 0 - открыть тему голосования: ";
    uint8_t isClose;
    std::cin >> isClose;
    if (isClose > 1) isClose = 1;
    ReadWriteHelper::setByte(buffer, writeOffset, isClose);
}






