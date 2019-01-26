#include "primitive.h"

using namespace serialization;

DataSerializer::DataSerializer(uint8_t *buffer) : buffer(buffer), index(0) {}

void DataSerializer::write(uint32_t number) {
    for (int i = 0; i < 4; ++i) {
        buffer[index++] = static_cast<uint8_t>(number & 0xffu);
        number >>= 8;
    }
}

void DataSerializer::write(const std::string &string) {
    auto length = static_cast<uint32_t>(string.length());
    write(length);
    const auto *p = reinterpret_cast<const uint8_t*>(string.c_str());
    for (size_t i = 0; i < length; ++i) {
        buffer[index++] = p[i];
    }
}

void DataSerializer::write(request::RequestType type) {
    buffer[index++] = type;
}

void DataSerializer::write(response::Status status) {
    buffer[index++] = status;
}

void DataSerializer::write(const model::Email &email) {
    write(email.getAuthor());
    write(email.getRecipient());
    write(email.getTheme());
    write(email.getBody());
}

void DataSerializer::write(const model::EmailInfo &info) {
    write(info.getId());
    write(info.getAuthor());
    write(info.getTheme());
}

void DataSerializer::write(const std::vector<model::EmailInfo> &infos) {
    write(static_cast<uint32_t>(infos.size()));
    for (auto &info : infos) {
        write(info);
    }
}

DataDeserializer::DataDeserializer(const uint8_t *buffer) : buffer(buffer), index(0) {}

uint32_t DataDeserializer::parseUInt32() {
    uint32_t result = buffer[index + 3];
    for (size_t i = index + 3; i > index; --i) {
        result <<= 8;
        result += buffer[i - 1];
    }
    index += 4;
    return result;
}

std::string DataDeserializer::parseString() {
    size_t size = parseUInt32();
    const auto *p = reinterpret_cast<const char*>(buffer + index);
    std::string string(p, size);
    index += size;
    return string;
}

request::RequestType DataDeserializer::parseRequestType() {
    return static_cast<request::RequestType>(buffer[index++]);
}

response::Status DataDeserializer::parseResponseStatus() {
    return static_cast<response::Status>(buffer[index++]);
}

model::Email DataDeserializer::parseEmail() {
    auto author = parseString();
    auto recipient = parseString();
    auto theme = parseString();
    auto body = parseString();
    return model::Email(theme, body, author, recipient);
}

model::EmailInfo DataDeserializer::parseEmailInfo() {
    uint32_t id = parseUInt32();
    auto author = parseString();
    auto theme = parseString();
    return model::EmailInfo(id, author, theme);
}

std::vector<model::EmailInfo> DataDeserializer::parseEmailInfos() {
    uint32_t size = parseUInt32();
    std::vector<model::EmailInfo> result(size);
    for (size_t i = 0; i < size; ++i) {
        result.push_back(parseEmailInfo());
    }
    return result;
}
