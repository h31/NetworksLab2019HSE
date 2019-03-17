#include "network/client_socket.h"

namespace network {

    ClientSocket::ClientSocket(int descriptor)
        : descriptor_(descriptor),
          is_alive_(descriptor != INVALID_SOCKET) {}

    bool ClientSocket::is_alive() {
        return (descriptor_ != INVALID_SOCKET);
    }

    void ClientSocket::close() {
        ::close(descriptor_);
    }

    void ClientSocket::send_message(const std::string &message) {
        auto size = static_cast<uint32_t>(message.size());
        auto buffer = encode_number(size);
        write_to_socket(buffer.get(), INT_SIZE);
        write_to_socket(message.c_str(), size);
    }

    void ClientSocket::write_to_socket(const void *buffer, uint32_t size) {
        auto result = send(descriptor_, buffer, size, 0);
        if (result < 0) {
            throw socket_exception("Socket is unavailable for writing!");
        }
    }

    std::string ClientSocket::receive_message() {
        uint32_t size = read_message_size();
        std::unique_ptr<char[]> message_buf(new char[size]);
        read_to_buffer(reinterpret_cast<uint8_t *>(message_buf.get()), size);
        return std::string(message_buf.get(), size);
    }


    std::shared_ptr<uint8_t[]> ClientSocket::encode_number(uint32_t number) {
        std::shared_ptr<uint8_t[]> buffer(new uint8_t[INT_SIZE]);
        for (int i = 0; i < INT_SIZE; ++i) {
            buffer[i] = static_cast<uint8_t>(number & UCHAR_MAX);
            number >>= POWER_OF_TWO;
        }
        return buffer;
    }

    void ClientSocket::read_to_buffer(uint8_t *buffer, size_t size) {
        size_t offset = 0;
        while (size > 0) {
            ssize_t readed = read(descriptor_, buffer + offset, size);
            if (readed < 0) {
                throw socket_exception("Socket is unavailable for reading!");
            }
            offset += static_cast<size_t >(readed);
            size -= static_cast<size_t >(readed);
        }
    }

    uint32_t ClientSocket::read_message_size() {
        std::unique_ptr<uint8_t[]> size_buf(new uint8_t[INT_SIZE]);
        read_to_buffer(size_buf.get(), INT_SIZE);
        return parse_uint32(size_buf.get());
    }

    uint32_t ClientSocket::parse_uint32(const uint8_t *buffer) {
        uint32_t result = buffer[INT_SIZE - 1];
        for (size_t i = INT_SIZE - 1; i > 0; --i) {
            result <<= POWER_OF_TWO;
            result += buffer[i - 1];
        }
        return result;
    }

    ClientSocket::~ClientSocket() {
        if (is_alive()) {
            close();
        }
    }

} // namespace network