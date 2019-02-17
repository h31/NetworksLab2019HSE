

//
// Created by Olga Alehina on 2019-02-10.
//

#include "../include/ClientHandler.h"

ReadWriteHelper ClientHandler::readWriteHelper;

void ClientHandler::operator()(int clientSocketFd) {
    if (clientSocketFd < 0) {
        throw ClientHandlerException("ERROR on accept");
    }
    ssize_t countRead;
    uint8_t buffer[256];
    bzero(buffer, 256);
    countRead = read(clientSocketFd, buffer, sizeof(buffer));

    if (countRead < 1) {
        throw ClientHandlerException("ERROR reading from socket");
    }
    uint32_t requestType = readWriteHelper.get4Bytes(buffer, 0);
    ssize_t n;
    switch (requestType) {
        case 0: {
            Response response = addWallet(buffer, requestType, countRead);
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


Response ClientHandler::addWallet(uint8_t *inputBuffer, uint32_t type, ssize_t countRead) {
    uint8_t buffer[256];
    bzero(buffer, 256);

    //записываем тип
    readWriteHelper.set4Bytes(buffer, 0, type);
    int offset = 4;
    uint32_t isSuccess = 0;
    std::uint64_t walletNumber = 0;
    if (countRead < 36) {
        isSuccess = 0;
    } else {
        char password[32];
        bzero(password, 32);
        int offsetForChar = 4;
        for (char &i : password) {
            i = (char) readWriteHelper.getByte(inputBuffer, offsetForChar);
            offsetForChar++;
        }
        std::cout << password;
        std::string passwordString(password);
        mutex_.lock();
        walletNumber = data.getFreeNumber();
        Wallet wallet(walletNumber, passwordString, 0);
        ClientHandler::data.addNewWaller(wallet);
        mutex_.unlock();
        isSuccess = 1;
    }

    //записали результат
    readWriteHelper.set4Bytes(buffer, offset, isSuccess);
    offset += 4;

    //записали номер
    readWriteHelper.set8Bytes(buffer, offset, walletNumber);

    return {buffer, 256};
}

