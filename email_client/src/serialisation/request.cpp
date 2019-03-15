#include <serialization/request.h>

namespace serialization {

    RequestSerializer::RequestSerializer(const request::Request *request)
        : request_(request),
          buffer_(new uint8_t[request_->size() + INT_SIZE]),
          serializer_(buffer_.get()) {}

    encoded_message serialization::RequestSerializer::serialize() {
        auto type = request_->get_type();
        serializer_.write(request_->size());
        serializer_.write(type);
        serializer_.write(request_->get_author());
        switch (type) {
            case request::SEND_EMAIL: {
                auto &email = reinterpret_cast<const request::SendRequest *>(request_)->get_email();
                serializer_.write(email.get_author());
                serializer_.write(email.get_recipient());
                serializer_.write(email.get_theme());
                serializer_.write(email.get_body());
                break;
            }
            case request::GET_EMAIL: {
                serializer_.write(reinterpret_cast<const request::GetRequest *>(request_)->get_id());
                break;
            }
            default:
                break;
        }
        return std::make_pair(request_->size() + INT_SIZE, buffer_);
    }

} // namespace serialization