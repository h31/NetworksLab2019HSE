#ifndef FILE_SYSTEM_CLIENT_H
#define FILE_SYSTEM_CLIENT_H

#include <cstdint>

#include "message.h"

class FileSystemClient {
public:
    FileSystemClient(const char* host, uint16_t port_number);

    void uploadFile(const char *destUrl, const char *sourceUrl);

    void downloadFile(const char *sourceUrl);

    void getFilesList();

    void moveToUrl(const char *url);

    void quit();

private:
    int _sockfd;

    bool _sendMessage(Message message);

    Message _getResponse();

    void processUnknownResponse(Message message);

    bool _saveFile(const char *content, size_t size, const char* dest);

    void _writeInt32ToBuffer(char* dest, uint32_t n, size_t& shift);

    void _writeToBuffer(char* dest, const char *src, size_t size, size_t& shift);

    bool _write(const char* src, size_t size);

    bool _readInt32(uint32_t *dst);

    bool _read(char *dst, size_t size);
};


#endif //FILE_SYSTEM_CLIENT_H
