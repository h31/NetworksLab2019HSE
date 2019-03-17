#include <serialization/serialization.h>

namespace serialization {

    SerializedMessage::SerializedMessage(size_t size_, const std::shared_ptr<uint8_t[]> &message_)
        : size_(size_),
          message_(message_) {}

    size_t SerializedMessage::size() const {
        return size_;
    }

    const uint8_t *SerializedMessage::get_message() const {
        return message_.get();
    }

    Serializer::Serializer(const request::Request *request)
        : request_(request),
          buffer_(new uint8_t[request->size() + INT_SIZE]),
          buf_(buffer_.get()),
          index_(0) {}

    void Serializer::write(uint32_t number) {
        for (int i = 0; i < INT_SIZE; ++i) {
            buf_[index_++] = static_cast<uint8_t>(number & UCHAR_MAX);
            number >>= POWER_OF_TWO;
        }
    }

    void Serializer::write(const std::string &string) {
        auto length = static_cast<uint32_t>(string.length());
        write(length);
        const auto *data = reinterpret_cast<const uint8_t *>(string.c_str());
        for (size_t i = 0; i < length; ++i) {
            buf_[index_++] = data[i];
        }
    }

    void Serializer::write(request::RequestType type) {
        buf_[index_++] = type;
    }

    void Serializer::write(const email::Email &email) {
        write(email.get_author());
        write(email.get_recipient());
        write(email.get_theme());
        write(email.get_body());
    }

    SerializedMessage Serializer::serialize() {
        auto type = request_->get_type();
        write(request_->size());
        write(type);
        write(request_->get_author());
        switch (type) {
            case request::SEND_EMAIL: {
                auto &email = reinterpret_cast<const request::SendRequest *>(request_)->get_email();
                write(email);
                break;
            }
            case request::GET_EMAIL: {
                write(reinterpret_cast<const request::GetRequest *>(request_)->get_id());
                break;
            }
            default:
                break;
        }
        return SerializedMessage(request_->size() + INT_SIZE, buffer_);
    }

    Deserializer::Deserializer(const uint8_t *buffer, uint32_t offset)
        : buffer_(buffer),
          index_(offset) {}

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
        const char *p = reinterpret_cast<const char *>(buffer_ + index_);
        std::string string(p, size);
        index_ += size;
        return string;
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

    std::shared_ptr<response::Response> Deserializer::parse_response(request::RequestType type) {
        response::ResponseStatus status = parse_response_status();
        std::shared_ptr<response::Response> responseBody;
        if (status == response::OK) {
            switch (type) {
                case request::SEND_EMAIL:
                    return std::make_shared<response::SendResponse>();
                case request::CHECK_EMAIL: {
                    std::vector<email::EmailInfo> infos = parse_email_infos();
                    return std::make_shared<response::CheckResponse>(infos);
                }
                case request::GET_EMAIL: {
                    email::Email email = parse_email();
                    return std::make_shared<response::GetResponse>(email);
                }
                default:
                    break;
            }
        }
        std::string message = parse_string();
        return std::make_shared<response::BadResponse>(message);
    }

} // namespace serialization