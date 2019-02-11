

//
// Created by Olga Alehina on 2019-02-10.
//

#include "../include/ClientHandler.h"


void ClientHandler::operator()(int clientSocketFd) {
    if (clientSocketFd < 0) {
        throw ClientHandlerException("ERROR on accept");
    }
    ssize_t countRead;
    int32_t requestType = 0;

    countRead = read(clientSocketFd, &requestType, sizeof(requestType));

    if (countRead < sizeof(requestType)) {
        throw ClientHandlerException("ERROR reading from socket");
    }
    ssize_t n;
    switch (requestType) {
        case 0: {
            Response response = addWallet(clientSocketFd, 1);
            n = write(clientSocketFd, response.buffer, response.count);
            break;
        }
        case 1: {
            Response response = getWalletNumbers(clientSocketFd, 1);
            n = write(clientSocketFd, response.buffer, response.count);
            break;
        }
        default : {
            n = write(clientSocketFd, "I got your message", 18);
            break;
        }
    }
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
}

Response ClientHandler::getWalletNumbers(int clientSocketFd, uint32_t type) {

    mutex_.lock_shared();
    std::set<uint64_t> walletNumbers = ClientHandler::data.getNumbers();
    mutex_.unlock_shared();

    // тип: 4 байта ; размер : 8 байт ; 8 * на кол-во всех номеров
    const auto bufferSize = static_cast<uint32_t>(4 + 8 + 8 * walletNumbers.size());
    auto buffer = new uint8_t[bufferSize];
    bzero(buffer, bufferSize);

    //записываем тип
    memcpy(buffer, &type, sizeof(type));
    int shift = sizeof(type);

    //записываем кол-во кошельков
    uint64_t size = walletNumbers.size();
    memcpy(buffer + shift, &size, sizeof(size));
    shift += sizeof(size);

    //записываем все номера кошельков
    for (auto walletNumber : walletNumbers) {
        memcpy(buffer + shift, &walletNumber, sizeof(walletNumber));
        shift += sizeof(walletNumber);
    }

    return {buffer, bufferSize};
}


Response ClientHandler::addWallet(int clientSocketFd, uint32_t type) {

    // тип: 4 байта ; результат: 4 байтa ; номер: 8
    const auto bufferSize = static_cast<uint32_t>(4 + 4 + 8);
    auto buffer = new uint8_t[bufferSize];
    bzero(buffer, bufferSize);

    //записываем тип
    memcpy(buffer, &type, sizeof(type));
    int shift = sizeof(type);

    uint32_t isSuccess = 0;
    std::uint64_t walletNumber = 0;


    try {
        ssize_t countRead;
        char password[32];
        std::cout << sizeof(password) << std::endl;
        countRead = read(clientSocketFd, &password, sizeof(password));
        if (countRead < sizeof(password)) {
            throw ClientHandlerException("ERROR reading from socket");
        }
        std::string passwordString(password);
        mutex_.lock();
        walletNumber = data.getFreeNumber();
        Wallet wallet(walletNumber, passwordString, 0);
        ClientHandler::data.addNewWaller(wallet);
        mutex_.unlock();
        isSuccess = 1;
    }
    catch (ClientHandlerException e) {
        isSuccess = 0;
    }

    //записали результат
    memcpy(buffer + shift, &isSuccess, sizeof(isSuccess));
    shift += sizeof(isSuccess);

    //записали номер
    memcpy(buffer + shift, &walletNumber, sizeof(walletNumber));
    shift += sizeof(walletNumber);

    return {buffer, bufferSize};
}

