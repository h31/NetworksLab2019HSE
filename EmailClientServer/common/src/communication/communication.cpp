#include <netinet/in.h>
#include <unistd.h>
#include <memory>
#include <communication/communication.h>
#include <serialization/request.h>
#include "serialization/response.h"
#include "serialization/primitive.h"
#include <sys/socket.h>

using namespace serialization;
using namespace std;

uint32_t communication::readMessageSize(int socket) {
    uint8_t messageSizeBuffer[INT_SIZE];
    readToBuffer(socket, messageSizeBuffer, INT_SIZE);
    DataDeserializer dataDeserializer(messageSizeBuffer);
    return dataDeserializer.parseUInt32();
}

void communication::readToBuffer(int socket, uint8_t *buffer, size_t size) {
    ssize_t valread = read(socket, buffer, size);
    if (valread < 0) {
        throw "Can't read from socket";
    }
}

void communication::writeToSocket(int socket, std::pair<uint32_t, std::shared_ptr<uint8_t[]>> message) {
    uint32_t messageSize = message.first;
    shared_ptr<uint8_t[]> &buffer = message.second;
    send(socket, buffer.get(), messageSize, 0);
}

void communication::sendResponse(int socket, std::unique_ptr<response::Response> &&response) {
    ResponseSerializer serializer(response.get());
    communication::writeToSocket(socket, serializer.serialize());
}

void communication::sendRequest(int socket, std::unique_ptr<request::Request> &&request) {
    RequestSerializer serializer(request.get());
    communication::writeToSocket(socket, serializer.serialize());
}

std::unique_ptr<uint8_t[]> communication::receiveMessage(int socket) {
    uint32_t messageSize = readMessageSize(socket);
    unique_ptr<uint8_t[]> buffer(new uint8_t[messageSize]);
    readToBuffer(socket, buffer.get(), messageSize);
    return buffer;
}
