#include <serialization/serialization.h>

namespace serialization {

    Serializer::Serializer(uint8_t *buffer) : buffer_(buffer), index_(0) {}

    void Serializer::write(uint32_t number) {
        for (int i = 0; i < INT_SIZE; ++i) {
            buffer_[index_++] = static_cast<uint8_t>(number & UCHAR_MAX);
            number >>= POWER_OF_TWO;
        }
    }

    encoded_message Serializer::serialize(const request::SendRequest &) {
        return {};
    }

    encoded_message Serializer::serialize(const request::GetRequest &) {
        return {};
    }

    encoded_message Serializer::serialize(const request::CheckRequest &) {
        return {};
    }

    void Serializer::write(const std::string &string) {
        auto length = static_cast<uint32_t>(string.length());
        write(length);
        const auto *data = reinterpret_cast<const uint8_t*>(string.c_str());
        for (size_t i = 0; i < length; ++i) {
            buffer_[index_++] = data[i];
        }
    }

    void Serializer::write(request::RequestType type) {
        buffer_[index_++] = type;
    }

    void Serializer::write(response::ResponseStatus status) {
        buffer_[index_++] = status;
    }

    void Serializer::write(const email::Email &email) {
        write(email.get_author());
        write(email.get_recipient());
        write(email.get_theme());
        write(email.get_body());
    }

    void Serializer::write(const email::EmailInfo &info) {
        write(info.get_id());
        write(info.get_author());
        write(info.get_theme());
    }

    void Serializer::write(const std::vector<email::EmailInfo> &infos) {
        write(static_cast<uint32_t>(infos.size()));
        for (auto &info : infos) {
            write(info);
        }
    }

    Deserializer::Deserializer(const uint8_t *buffer, uint32_t offset) : buffer_(buffer), index_(offset) {}

    uint32_t Deserializer::parse_uint32() {
        uint32_t result = buffer_[index_ + INT_SIZE - 1];
        for (size_t i = index_ + INT_SIZE - 1; i > index_; --i) {
            result <<= POWER_OF_TWO;
            result += buffer_[i - 1];
        }
        index_ += INT_SIZE;
        return result;
    }

    std::string Deserializer::parse_string() {
        size_t size = parse_uint32();
        const char *p = reinterpret_cast<const char*>(buffer_ + index_);
        std::string string(p, size);
        index_ += size;
        return string;
    }

    request::RequestType Deserializer::parse_request_type() {
        return static_cast<request::RequestType>(buffer_[index_++]);
    }

    response::ResponseStatus Deserializer::parse_response_status() {
        return static_cast<response::ResponseStatus>(buffer_[index_++]);
    }

    email::Email Deserializer::parse_email() {
        auto author = parse_string();
        auto recipient = parse_string();
        auto theme = parse_string();
        auto body = parse_string();
        return email::Email(recipient, author, theme, body);
    }

    email::EmailInfo Deserializer::parse_email_info() {
        uint32_t id = parse_uint32();
        auto author = parse_string();
        auto theme = parse_string();
        return email::EmailInfo(id, author, theme);
    }

    std::vector<email::EmailInfo> Deserializer::parse_email_infos() {
        uint32_t size = parse_uint32();
        std::vector<email::EmailInfo> result;
        result.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            result.push_back(parse_email_info());
        }
        return result;
    }

} // namespace serialization