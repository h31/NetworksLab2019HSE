#pragma once

#include "pch.h"
#include "socket_exception.h"

namespace network {

    class ClientSocket {
    public:
        explicit ClientSocket(int descriptor = -1);

        ~ClientSocket();

        bool is_alive();

        void close();

        void send_message(const std::string &message);

        std::string receive_message();

    private:
        int descriptor_;
        bool is_alive_;

        void write_to_socket(const void *buffer, uint32_t size);

        void read_to_buffer(uint8_t *buffer, size_t size);

        uint32_t read_message_size();

        std::shared_ptr<uint8_t[]> encode_number(uint32_t number);

        uint32_t parse_uint32(const uint8_t *buffer);
    };


} // namespace network