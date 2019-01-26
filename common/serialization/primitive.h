#ifndef COMMON_SERIALIZATION_PRIMITIVE_H
#define COMMON_SERIALIZATION_PRIMITIVE_H

#include <string>
#include <model/response.h>
#include <model/request.h>

namespace serialization {
    class DataSerializer {
    private:
        uint8_t *buffer;
        size_t index;
    public:
        explicit DataSerializer(uint8_t *buffer);

        void write(uint32_t number);

        void write(const std::string &string);

        void write(request::RequestType type);

        void write(response::Status status);

        void write(const model::Email &email);

        void write(const model::EmailInfo &info);

        void write(const std::vector<model::EmailInfo> &infos);
    };

    class DataDeserializer {
    private:
        const uint8_t *buffer;
        size_t index;
    public:
        explicit DataDeserializer(const uint8_t *buffer);

        uint32_t parseUInt32();

        std::string parseString();

        request::RequestType parseRequestType();

        response::Status parseResponseStatus();

        model::Email parseEmail();

        model::EmailInfo parseEmailInfo();

        std::vector<model::EmailInfo> parseEmailInfos();
    };
}

#endif //COMMON_SERIALIZATION_PRIMITIVE_H
