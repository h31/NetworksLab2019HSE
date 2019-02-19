#include <Response.h>
#include <unistd.h>
#include <iostream>

#include "Response.h"

Response::Response(Response::ResponseType type) : type(type) {  }

Response Response::readResponse(int socketfd) {
    ResponseType type;
    if (read(socketfd, &type, sizeof(type)) != sizeof(type)) {
        return Response(ResponseType::DISCONNECT);
    }
    Response resp(type);
    switch (type) {
        case ResponseType::SUCCESS:
        case ResponseType::DISCONNECT:
            return resp;
        case ResponseType::ERROR:
            return resp.readField(socketfd, RequestField::Type::STRING);
        case ResponseType::RATING_LIST: {
            resp.readField(socketfd, RequestField::Type::INT);
            uint32_t cnt = resp.fields.back().getInt();
            for (uint8_t i = 0; i < cnt && !resp.isDisconnect(); ++i) {
                resp.readField(socketfd, RequestField::Type::STRING)
                        .readField(socketfd, RequestField::Type::INT)
                        .readField(socketfd, RequestField::Type::BYTE);
            }
            return resp;
        }
        case ResponseType::RATING_STATS:  {
            resp.readField(socketfd, RequestField::Type::STRING)
                    .readField(socketfd, RequestField::Type::BYTE)
                    .readField(socketfd, RequestField::Type::BYTE);
            uint32_t cnt = resp.fields.back().getByte();
            for (uint8_t i = 0; i < cnt && !resp.isDisconnect(); ++i) {
                resp.readField(socketfd, RequestField::Type::STRING)
                    .readField(socketfd, RequestField::Type::INT);
            }
            return resp;
        }
    }
}

Response &Response::readField(int socketfd, RequestField::Type rtype) {
    RequestField field(rtype);
    if (isDisconnect()) {
        return *this;
    }
    if (!field.read(socketfd)) {
        type = ResponseType::DISCONNECT;
        fields.clear();
    } else {
        fields.push_back(field);
    }
    return *this;
}

bool Response::isError() {
    return type == ResponseType::ERROR;
}

bool Response::isDisconnect() {
    return type == ResponseType::DISCONNECT;
}

std::string Response::getError() {
    return fields[0].getString();
}

void Response::print() {
    switch (type) {
        case ResponseType::RATING_LIST: {
            std::cout << "Available ratings:" << std::endl;
            uint32_t cnt = fields[0].getInt();
            for (uint8_t i = 1; i < 1 + 3 * cnt; i += 3) {
                std::cout << "#" << fields[i + 1].getInt() << ": ";
                std::cout << fields[i].getString();
                if (!fields[i + 2].getByte()) {
                    std::cout << " (closed)";
                }
                std::cout << std::endl;
            }
            break;
        }
        case ResponseType::RATING_STATS: {
            std::cout << "Rating " << fields[0].getString();
            if (!fields[1].getByte()) {
                std::cout << " (closed)";
            }
            std::cout << ":" << std::endl;
            uint32_t cnt = fields[2].getByte();
            for (uint8_t i = 3; i < 3 + 2 * cnt; i += 2) {
                std::cout << " #" << (i - 3) / 2 << " " << fields[i].getString() << ": ";
                std::cout << fields[i + 1].getInt() << std::endl;
            }
            break;
        }
        case ResponseType::ERROR:
            std::cerr << "Error: " << getError() << std::endl;
        default:
            break;
    }
}

bool Response::checkDisconnect() {
    if (isDisconnect()) {
        std::cerr << "You have been disconnected" << std::endl;
    }
    return isDisconnect();
}

Response Response::ResponseDisconnect() {
    return Response(ResponseType::DISCONNECT);
}
