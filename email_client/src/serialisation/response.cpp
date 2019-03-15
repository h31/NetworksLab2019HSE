#include "serialization/serialization.h"
#include "serialization/response.h"

namespace serialization {

    ResponseDeserializer::ResponseDeserializer(const uint8_t *buffer, uint32_t offset) : deserializer_(buffer, offset) {}

    std::shared_ptr<response::Response> ResponseDeserializer::parseResponse(request::RequestType type) {
        response::ResponseStatus status = deserializer_.parse_response_status();
        std::shared_ptr<response::Response> responseBody;
        if (status == response::OK) {
            switch (type) {
                case request::SEND_EMAIL:
                    return std::make_shared<response::SendResponse>();
                case request::CHECK_EMAIL: {
                    std::vector<email::EmailInfo> infos = deserializer_.parse_email_infos();
                    return std::make_shared<response::CheckResponse>(infos);
                }
                case request::GET_EMAIL: {
                    email::Email email = deserializer_.parse_email();
                    return std::make_shared<response::GetResponse>(email);
                }
                default: break;
            }
        }
        std::string message = deserializer_.parse_string();
        return std::make_shared<response::BadResponse>(message);
    }

} // namespace serialization