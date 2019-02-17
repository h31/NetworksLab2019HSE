#include <utility>

#include "client.h"
#include "ReadWriteHelper.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <unistd.h>

#include <string.h>


Client::Client(char* host_name, int port_number){
	this->host_name = host_name;
	this->port_number = port_number;
}

ReadWriteHelper Client::readWriteHelper;


bool Client::connect(){
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        return false;
    }

    struct hostent *server = gethostbyname(host_name);

    if (server == nullptr) {
        return false;
    }

    struct sockaddr_in serv_addr{};
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(port_number);
    if (::connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }
    return true;
}


uint32_t Client::sendRequest(){
    std::cout << "0 -- add user\n1 -- get all wallets\n2 -- sent to user\n3 -- ask from user\n"
         << "4 -- confirm sent request\n5 -- check money\n6 -- check sent requests\n7 -- disconnect\n\n Enter message type: ";

    uint32_t message_type;
    std::cin >> message_type;
    
    bzero(buffer, 128);
    readWriteHelper.set4Bytes(buffer, 0, message_type);

    switch(message_type) {
        case 0: 
            addUser();
            break;
        case 1:
            break;
        case 2: 
            sendMoney();
            break;
        case 3: 
            requestMoney();
            break;
        case 4:
        	confirmRequest();
            break;
        case 5: 
            checkMoney();
            break;
        case 6:
            checkRequests(); 
            break;
        case 7:
            return 7;
        default:
            std::cout << "wrong message type";
            return message_type;
    }
    ssize_t n = write(sockfd, buffer, sizeof(buffer));

    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
    return message_type;
}

void Client::getResponse(uint32_t sent_message_type) {
    ssize_t n;
    if (sent_message_type == 1) {
        bzero(buffer, 1024);
        n = read(sockfd, buffer, 1024);
    } else {
        bzero(buffer, 256);
        n = read(sockfd, buffer, 256);
    }

    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    uint32_t message_type = shortNumberResponse(0);

    switch(message_type) {
        case 0: 
            addUserResponse();
            break;
        case 1:
            getAllWalletsResponse();
            break;
        case 2: 
            successResponse();
            break;
        case 3: 
            successResponse();
            break;
        case 4:
        	successResponse();
            break;
        case 5: 
            std::cout << "You have " << longNumberResponse(4);
            break;
        case 6:
            checkRequestsResponse(); 
            break;
        default:
            std::cout << "wrong message type";
            return;
    }
}  

void Client::addUser() {
	readPassword(4);
}

void Client::sendMoney() {
	readClientWallet(4); 
    readWallet(4 + 8, "Enter wallet number to send money: ");
    readPassword(4 + 2 * 8);
    readMoney(4 + 2 * 8 + 32);
}

void Client::requestMoney() {
    readClientWallet(4);
    readWallet(4 + 8, "Enter wallet number to ask money: ");
    readPassword(4 + 2 * 8);
    readMoney(4 + 2 * 8 + 32);
}

void Client::confirmRequest() {
	readClientWallet(4);
    readRequestId(4 + 8);
    readPassword(4 + 2 * 8);
    readConfirm(4 + 2 * 8 + 32);
}

void Client::checkMoney() {
	readClientWallet(4);
    readPassword(4 + 8);
}

void Client::checkRequests() {
	readClientWallet(4);
    readPassword(4 + 8);
}

void Client::readPassword(int offset) {
	std::string password;    
    while(true) {
		std::cout << "Enter password: ";
        std::cin >> password;
        if (password.size() > 32) {
            std::cout << "Too long";
        } else {
        	break;
        }
    }
    for (char &i : password) {
        readWriteHelper.setByte(buffer, offset, static_cast<uint32_t>(i));
        offset++;
    }
}

void Client::readClientWallet(int offset) {
	readWallet(offset, "Enter your wallet number: ");
}

void Client::readWallet(int offset, std::string whoseWallet) {
	std::cout << whoseWallet;
    uint64_t walletNumber;
    std::cin >> walletNumber;
    readWriteHelper.set8Bytes(buffer, offset, walletNumber);
}

void Client::readMoney(int offset) {
    std::cout << "Enter how much to send: ";
    uint32_t money;
    std::cin >> money;
    readWriteHelper.set4Bytes(buffer, offset, money);
}

void Client::readConfirm(int offset) {
    std::cout << "Do you want to confirm request?\n 1 - yes\n 0 - no\n: ";
    uint32_t confirmation;
    std::cin >> confirmation;
    readWriteHelper.set4Bytes(buffer, offset, confirmation);
}

void Client::readRequestId(int offset) {
	std::cout << "Enter request ID: ";
    uint64_t walletNumber;
    std::cin >> walletNumber;
    readWriteHelper.set8Bytes(buffer, offset, walletNumber);
}

void Client::addUserResponse() {
	successResponse();
	uint64_t number = longNumberResponse(8);
	std::cout << "Your wallet number: " << number << "\n";
}

uint64_t Client::longNumberResponse(int offset) {
    return readWriteHelper.get8Bytes(buffer, offset);
}

uint32_t Client::shortNumberResponse(int offset) {
    return readWriteHelper.get4Bytes(buffer, offset);
}

void Client::getAllWalletsResponse() {
    uint64_t walletsNumber = longNumberResponse(4);
    for (int i = 0; i < walletsNumber; i++) {
    	std::cout << longNumberResponse(4 + 8 * (i + 1)) << "\n";
    }
}


void Client::successResponse() {
	int32_t success = shortNumberResponse(4);
	if (success == 1) {
		std::cout << "Success\n";
	} else {
		std::cout << "Fail\n";
	}
}

void Client::checkRequestsResponse() {
	int64_t requestNumber = longNumberResponse(4);
    for (int i = 0; i < requestNumber; i++) {
    	std::cout << "id: " << longNumberResponse(8 + 24 * i) 
    	<< " from: " << longNumberResponse(8 + 24 * i + 8) 
    	<< "money: " << longNumberResponse(8 + 24 * i + 16) << "\n";
    }
}