#pragma once

#include <sys/socket.h>
#include <serialization/serialization.h>
#include <serialization/request.h>
#include <serialization/response.h>

#include "pch.h"
#include "response.h"
#include "request.h"

namespace network {

    struct socket_exception : std::exception {
        explicit socket_exception(std::string msg = "");

        const char *what() const noexcept override;

    private:
        std::string msg_;
    };

    class ClientSocket {
    public:

        ClientSocket(const std::string &host, uint16_t port);

        ~ClientSocket() = default;

        bool open_connection();

        void close_connection();

        std::shared_ptr<response::Response> sendRequest(std::unique_ptr<request::Request> & request);

        int get_descriptor() const;

    private:
        int descriptor_;
        std::string server_address_;
        uint16_t port_;

        void write_message(const std::pair<uint32_t,  std::shared_ptr<uint8_t[]>>& message);

        std::unique_ptr<uint8_t[]> receive_message();

        uint32_t read_message_size();

        void read_to_buffer(uint8_t *buffer, size_t size);

    };

}