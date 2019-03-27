#include <Response.h>
#include <unistd.h>
#include <iostream>

#include "Response.h"

Response::Response(Response::ResponseType type) : type(type) {  }

Response Response::readResponse(SocketReader &reader) {
    ResponseType type;
    if (!reader.readBytes(&type, sizeof(type))) {
        return Response(ResponseType::DISCONNECT);
    }
    Response resp(type);
    switch (type) {
        case ResponseType::SUCCESS:
        case ResponseType::DISCONNECT:
            return resp;
        case ResponseType::ERROR:
            return resp.readField(reader, RequestField::Type::STRING);
        case ResponseType::RATING_LIST: {
            resp.readField(reader, RequestField::Type::INT);
            uint32_t cnt = resp.fields.back().getInt();
            for (uint8_t i = 0; i < cnt && !resp.isDisconnect(); ++i) {
                resp.readField(reader, RequestField::Type::STRING)
                        .readField(reader, RequestField::Type::INT)
                        .readField(reader, RequestField::Type::BYTE);
            }
            return resp;
        }
        case ResponseType::RATING_STATS:  {
            resp.readField(reader, RequestField::Type::STRING)
                    .readField(reader, RequestField::Type::BYTE)
                    .readField(reader, RequestField::Type::BYTE);
            uint32_t cnt = resp.fields.back().getByte();
            for (uint8_t i = 0; i < cnt && !resp.isDisconnect(); ++i) {
                resp.readField(reader, RequestField::Type::STRING)
                    .readField(reader, RequestField::Type::INT);
            }
            return resp;
        }
        default:
            return exitResponse();
    }
}

Response &Response::readField(SocketReader &reader, RequestField::Type rtype) {
    RequestField field(rtype);
    if (isDisconnect()) {
        return *this;
    }
    if (!field.read(reader)) {
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
            for (uint8_t i = 0; i < cnt; ++i) {
                auto index = static_cast<uint8_t>(3 + 2 * i);
                std::cout << " #" << i << " " << fields[index].getString() << ": ";
                std::cout << fields[index + 1].getInt() << std::endl;
            }
            break;
        }
        case ResponseType::ERROR:
            std::cerr << "Error: " << getError() << std::endl;
        default:
            break;
    }
}

bool Response::checkExit() {
    if (isDisconnect()) {
        std::cerr << "You have been disconnected" << std::endl;
    }
    return isExit() || isDisconnect();
}

Response Response::ResponseDisconnect() {
    return Response(ResponseType::DISCONNECT);
}

Response Response::exitResponse() {
    return Response(ResponseType::EXIT);
}

bool Response::isExit() {
    return type == ResponseType::EXIT;
}
