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

uint32_t util::readMessageSize(int socket) {
    uint8_t messageSizeBuffer[INT_SIZE];
    readToBuffer(socket, messageSizeBuffer, INT_SIZE);
    DataDeserializer dataDeserializer(messageSizeBuffer);
    return dataDeserializer.parseUInt32();
}

void util::readToBuffer(int socket, uint8_t *buffer, size_t size) {
    size_t offset = 0;
    while (size > 0) {
        ssize_t valRead = read(socket, buffer + offset, size);
        if (valRead < 0) {
            throw "Can't read from socket";
        }
        offset += valRead;
        size -= valRead;
    }
}

void util::writeToSocket(int socket, std::pair<uint32_t, std::shared_ptr<uint8_t[]>> message) {
    uint32_t messageSize = message.first;
    shared_ptr<uint8_t[]> &buffer = message.second;
    send(socket, buffer.get(), messageSize, 0);
}

void util::sendResponse(int socket, std::unique_ptr<response::Response> &&response) {
    ResponseSerializer serializer(response.get());
    util::writeToSocket(socket, serializer.serialize());
}

void util::sendRequest(int socket, std::unique_ptr<request::Request> &&request) {
    RequestSerializer serializer(request.get());
    util::writeToSocket(socket, serializer.serialize());
}

std::unique_ptr<uint8_t[]> util::receiveMessage(int socket) {
    uint32_t messageSize = readMessageSize(socket);
    unique_ptr<uint8_t[]> buffer(new uint8_t[messageSize]);
    readToBuffer(socket, buffer.get(), messageSize);
    return buffer;
}
