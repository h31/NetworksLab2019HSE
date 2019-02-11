#include <cstdio>
#include <cstdio>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "client.h"
#include "message.h"

FileSystemClient::FileSystemClient(const char *host, uint16_t port_number) {
    /* Create a socket point */
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    hostent* server = gethostbyname(host);

    if (_sockfd < 0) {
        std::cerr << "ERROR opening socket\n";
        exit(1);
    }

    if (server == nullptr) {
        std::cerr << "ERROR, no such host\n";
        exit(0);
    }

    sockaddr_in serv_addr{};
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(port_number);

    /* Now connect to the server */
    if (connect(_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "ERROR connecting";
        exit(1);
    }
}

void FileSystemClient::uploadFile(const char *destUrl, const char *sourceUrl) {
    size_t destSize = strlen(destUrl);
    size_t sourceSize = strlen(sourceUrl);
    char data[destSize + sourceSize + 2 * sizeof(uint32_t)];
    size_t shift = 0;
    _writeInt32ToBuffer(data, static_cast<uint32_t>(destSize), shift);
    _writeToBuffer(data, destUrl, destSize, shift);
    _writeInt32ToBuffer(data, static_cast<uint32_t>(sourceSize), shift);
    _writeToBuffer(data, sourceUrl, sourceSize, shift);
    Message message = Message(100, data);
    _sendMessage(message);
    Message response = _getResponse();
    if (response.type == 101) {
        std::cout << "File was uploaded: " << response.content << '\n';
    } else if (response.type == 102) {
        std::cout << "Failed to upload file: " << response.content << '\n';
    } else {
        processUnknownResponse(response);
    }
}

void FileSystemClient::downloadFile(const char *sourceUrl) {
    Message message = Message(200, sourceUrl);
    _sendMessage(message);
    Message response = _getResponse();
    if (response.type == 201) {
        std::cout << "Enter file location:";
        std::string filename;
        getline(std::cin, filename);
        if (_saveFile(response.content, strlen(response.content), filename.data())) {
            std::cout << "File " << filename << " was successfully saved";
        } else {
            std::cout << "Error while saving file: " << filename;
        };
    } else if (response.type == 202) {

    } else {
        processUnknownResponse(response);
    }
}

void FileSystemClient::getFilesList() {
    Message message = Message(300, "");
    _sendMessage(message);
    uint32_t type;
    _readInt32(&type);
    if (type == 301) {
        uint32_t size, filesNumber, nameSize;
        _readInt32(&size);
        _readInt32(&filesNumber);
        for (uint32_t i = 0; i < filesNumber; i++) {
            _readInt32(&nameSize);
            char data[nameSize];
            _read(data, nameSize);
            std::cout << data << '\n';
        }
    } else {
        std::cout << "Unknown response code " <<  type << '\n';
    }
}

void FileSystemClient::moveToUrl(const char *url) {
    Message message = Message(400, url);
    _sendMessage(message);
    Message response = _getResponse();
    if (response.type == 401) {
        std::cout << "Current location: " << response.content << '\n';
    } else if (response.type == 402) {
        std::cout << "Location was not found: " << response.content << '\n';
    } else {
        processUnknownResponse(response);
    }
}

void FileSystemClient::quit() {
    std::cout << "Closing connection\n";
    shutdown(_sockfd, SHUT_RDWR);
    std::cout << "Connection was closed\n";
}

bool FileSystemClient::_sendMessage(Message message) {
    size_t size = strlen(message.content);
    size_t buf_size = size + sizeof(uint32_t) * 2;
    char data[buf_size];
    size_t shift = 0;
    _writeInt32ToBuffer(data, message.type, shift);
    _writeInt32ToBuffer(data, static_cast<uint32_t>(size), shift);
    _writeToBuffer(data, message.content, size, shift);
    _write(data, buf_size);
    return true;
}

Message FileSystemClient::_getResponse() {
    uint32_t type, size;
    if (FileSystemClient::_readInt32(&type) && FileSystemClient::_readInt32(&size)) {
        char buf[size];
        if (size == 0 || !FileSystemClient::_read(buf, size)) {
            return Message();
        } else {
            return Message(type, buf);
        }
    }
    return Message();
}

bool FileSystemClient::_saveFile(const char *content, size_t size, const char *dest) {
    return false;
}

void FileSystemClient::processUnknownResponse(Message message) {
    std::cout << "Unknown response code " <<  message.type << '\n';
}

/* WRITING */

void FileSystemClient::_writeInt32ToBuffer(char *dest, uint32_t n, size_t& shift) {
    n = htonl(n);
    memcpy(dest + shift, &n, sizeof(uint32_t));
    shift += sizeof(uint32_t);
}

void FileSystemClient::_writeToBuffer(char *dest, const char *src, size_t size, size_t& shift) {
    memcpy(dest + shift, src, size);
    shift += size;
}

bool FileSystemClient::_write(const char *src, size_t size) {
    ssize_t written = ::write(_sockfd, src, size);
    if (written < 0) {
        std::cerr << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

/* READING */

bool FileSystemClient::_readInt32(uint32_t *dst) {
    if (FileSystemClient::_read(reinterpret_cast<char *> (dst), sizeof(uint32_t))) {
        *dst = ntohl(*dst);
        return true;
    }
    return false;
}

bool FileSystemClient::_read(char *dst, size_t size) {
    for (ssize_t read = 0; size; read = ::read(_sockfd, dst, size)) {
        if (read < 0) {
            std::cerr << "Error while reading\n";
            return false;
        } else {
            dst += read;
            size -= read;
        }
    }
    return true;
}