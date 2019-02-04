#include <unistd.h>
#include <util.h>
#include <sys/socket.h>
#include <iostream>

using namespace std;
using namespace nlohmann;


nlohmann::json util::receiveMessage(int socket) {
    unique_ptr<uint8_t[]> sizeBuffer(new uint8_t[INT_SIZE]);
    readFromSocket(socket, sizeBuffer.get(), INT_SIZE);
    uint32_t messageSize = decodeNumber(sizeBuffer.get());
    unique_ptr<char[]> messageBuffer(new char[messageSize]);
    readFromSocket(socket, messageBuffer.get(), messageSize);
    string message(messageBuffer.get(), messageSize);
    cout << messageSize << ": " << message << endl;
    try {
        return json::parse(message);
    } catch (json::parse_error &exception) {
        throw "Error during parsing message";
    }
}

void util::sendMessage(int socket, const json &message) {
    string stringMessage = message.dump();
    auto size = static_cast<uint32_t>(stringMessage.size());
    auto sizeBuffer = encodeNumber(size);
    cout << size << ": " << stringMessage << endl;
    writeToSocket(socket, sizeBuffer.get(), INT_SIZE);
    writeToSocket(socket, stringMessage.c_str(), size);
}

void util::writeToSocket(int socket, const void *buffer, uint32_t size) {
    auto result = send(socket, buffer, size, 0);
    if (result < 0) {
        throw "Can't write to socket";
    }
}

void util::readFromSocket(int socket, void *buffer, uint32_t size) {
    ssize_t valRead = read(socket, buffer, size);
    if (valRead < 0) {
        throw "Can't read from socket";
    }
}

uint32_t util::decodeNumber(const uint8_t *buffer) {
    uint32_t result = buffer[INT_SIZE - 1];
    for (size_t i = INT_SIZE - 1; i > 0; --i) {
        result <<= 8;
        result += buffer[i - 1];
    }
    return result;
}

std::shared_ptr<uint8_t[]> util::encodeNumber(uint32_t number) {
    shared_ptr<uint8_t[]> buffer(new uint8_t[util::INT_SIZE]);
    for (int i = 0; i < INT_SIZE; ++i) {
        buffer[i] = static_cast<uint8_t>(number & 0xffu);
        number >>= 8;
    }
    return buffer;
}

nlohmann::json util::communicate(int socket, const nlohmann::json &message) {
    sendMessage(socket, message);
    return receiveMessage(socket);
}
