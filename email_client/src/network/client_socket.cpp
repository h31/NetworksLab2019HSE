#include <network/client_socket.h>

namespace network {

    socket_exception::socket_exception(std::string msg) : msg_(std::move(msg)) {}

    const char *socket_exception::what() const noexcept {
        return msg_.data();
    }

    ClientSocket::ClientSocket(const std::string &server_address, uint16_t port)
        : descriptor_(-1), server_address_(server_address), port_(port) {}

    void ClientSocket::open_connection() {
        struct sockaddr_in serv_addr;

        if ((descriptor_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            descriptor_ = -1;
            throw socket_exception("Socket creation error!");
        }

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port_);

        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, server_address_.c_str(), &serv_addr.sin_addr) <= 0) {
            descriptor_ = -1;
            throw socket_exception("Invalid address / Address not supported!");
        }
        if (connect(descriptor_, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr)) < 0) {
            descriptor_ = -1;
            throw socket_exception("Connection failed!");
        }
    }

    void ClientSocket::close_connection() {
        if (descriptor_ != -1) {
            ::close(descriptor_);
            descriptor_ = -1;
        }
    }

    std::shared_ptr<response::Response> ClientSocket::send_request(std::unique_ptr<request::Request> &request) {
        open_connection();

        serialization::Serializer serializer(request.get());
        write_message(serializer.serialize());

        auto buffer = receive_message();
        serialization::Deserializer deserializer(buffer.get());
        auto response = deserializer.parseResponse(request->get_type());
        close_connection();
        return response;
    }

    void ClientSocket::write_message(const serialization::SerializedMessage &message) {
        send(descriptor_, message.get_message(), message.size(), 0);
    }

    std::unique_ptr<uint8_t[]> ClientSocket::receive_message() {
        uint32_t messageSize = read_message_size();
        std::unique_ptr<uint8_t[]> buffer(new uint8_t[messageSize]);
        read_to_buffer(buffer.get(), messageSize);
        return buffer;
    }

    uint32_t ClientSocket::read_message_size() {
        uint8_t messageSizeBuffer[INT_SIZE];
        read_to_buffer(messageSizeBuffer, INT_SIZE);
        serialization::Deserializer deserializer(messageSizeBuffer);
        return deserializer.parse_uint32();
    }

    void ClientSocket::read_to_buffer(uint8_t *buffer, size_t size) {
        size_t offset = 0;
        while (size > 0) {
            ssize_t readed = read(descriptor_, buffer + offset, size);
            if (readed < 0) {
                throw socket_exception("Socket is unavailable for reading!");
            }
            offset += static_cast<size_t>(readed);
            size -= static_cast<size_t>(readed);
        }
    }

    ClientSocket::~ClientSocket() {
        if (descriptor_ == -1) {
            close_connection();
        }
    }

} // namespace network

