#include "operations.hpp"
#include <memory>
#include <string>
#include <cstring>

namespace protocol {
    // result -- 4 bytes
    // opertaion type -- 1 byte
    // status -- 1 byte
    // or 1 byte for status
    const size_t MAX_RESPONSE_SIZE = 4 + 1 + 1;

    // arg1 -- 4 bytes
    // arg2 or executation time -- 4 bytes
    // opertation type -- 1 byte
    const size_t REQUEST_SIZE = 4 + 4 + 1;

    struct Request {
        explicit Request(operations::Type type) : type_(type) {}

        virtual void toCharArray(char *buffer) = 0;

    protected:
        const operations::Type type_;

        void writeIntToCharArray(char *buffer, int32_t value, size_t startPosition) {
            const size_t end = startPosition + 4;
            for (size_t i = startPosition; i < end; ++i) {
                buffer[i] = static_cast<uint8_t >(value & 0xffu);
                value >>= 8;
            }
        }
    };

    struct LongOperationRequest : Request {
        LongOperationRequest(int32_t param, int32_t time, operations::Type type) :
                param_(param),
                time_(time),
                Request(type) {}


        void toCharArray(char *buffer) override {
            writeIntToCharArray(buffer, param_, 0);
            writeIntToCharArray(buffer, time_, 0);
            buffer[REQUEST_SIZE - 2] = type_;
        }

    private:
        const int32_t param_;
        const int32_t time_;

    };

    struct QuickOperationRequest : Request {
        QuickOperationRequest(int32_t param1, int32_t param2, operations::Type type) :
                param1_(param1),
                param2_(param2),
                Request(type) {}

        void toCharArray(char *buffer) override {
            writeIntToCharArray(buffer, param1_, 0);
            writeIntToCharArray(buffer, param2_, 0);
            buffer[REQUEST_SIZE - 2] = type_;
        }

    private:
        const int32_t param1_;
        const int32_t param2_;


    };


    int32_t parseInt(const char *buffer, const size_t start) {
        int32_t result = buffer[start + 4 - 1];
        for (size_t i = start + 4 - 1; i > start; --i) {
            result <<= 8;
            result += buffer[i - 1];
        }
        return result;
    }


    using namespace operations;

    namespace status {
        enum Status {
            Ok = 0,
            Fail = -1
        };

    }

    struct Response {
        explicit Response(status::Status stat) : stat_(stat) {};

        status::Status getStatus() {
            return stat_;
        }

    private:
        const status::Status stat_;
    };

    struct Ok : public Response {
        Ok(int32_t val, operations::Type type) : result_(val), type_(type), Response(status::Ok) {

        }

        int32_t getResult() {
            return result_;
        }

    private:
        int32_t result_;
        operations::Type type_;
    };

    struct Fail : public Response {
        Fail() :
                Response(status::Fail) {};
    };

    Response *parseResult(const int socket) {
        std::unique_ptr<char[]> buffer(new char[MAX_RESPONSE_SIZE]);

        ssize_t ssize = 0;
        while (ssize < MAX_RESPONSE_SIZE) {
            if (ssize == 1 && buffer[0] == status::Fail) {
                return new Fail();
            }

            ssize = read(socket, buffer.get() + ssize, MAX_RESPONSE_SIZE - ssize);
        }

        int32_t result = parseInt(buffer.get(), 0);
        char &operationType = buffer[MAX_RESPONSE_SIZE - 2];

        return new Ok(result, static_cast<Type>(operationType));
    }

}