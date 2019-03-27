#ifndef FILE_SYSTEM_MESSAGE_H
#define FILE_SYSTEM_MESSAGE_H

#include <string>

class Message {
public:
    enum Code {
        UNKNOWN_CODE = 1,
        UPLOAD_FILE = 100,
        UPLOAD_SUCCESS = 101,
        UPLOAD_LOCATION_UNAVAILABLE = 102,
        DOWNLOAD_FILE = 200,
        DOWNLOAD_SUCCESS = 201,
        DOWNLOAD_FILE_NOT_FOUND = 202,
        FILES_LIST = 300,
        FILES_LIST_SUCCESS = 301,
        MOVE_ON_FS = 400,
        MOVE_SUCCESS = 401,
        MOVE_LOCATION_NOT_FOUND = 402,
    };

    uint32_t type;
    std::string content;

    Message();

    explicit Message(uint32_t type);

    Message(uint32_t type, const char *content);
};


#endif //FILE_SYSTEM_MESSAGE_H
