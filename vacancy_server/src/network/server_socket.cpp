#include "network/server_socket.h"

namespace network {

    ServerSocket::ServerSocket(uint16_t port)
        : descriptor_(INVALID_SOCKET),
          port_(port),
          address_() {}

    void ServerSocket::open_connection() {
        int optval = 1;

        if ((descriptor_ = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            descriptor_ = INVALID_SOCKET;
            throw socket_exception("Socket creation failed!");
        }

        if (setsockopt(descriptor_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) {
            descriptor_ = -1;
            throw socket_exception("Setting socket flags failed!");
        }

        address_.sin_family = AF_INET;
        address_.sin_addr.s_addr = INADDR_ANY;
        address_.sin_port = htons(port_);

        if (bind(descriptor_, reinterpret_cast<struct sockaddr *>(&address_), sizeof(address_)) < 0) {
            descriptor_ = INVALID_SOCKET;
            throw socket_exception("Socket binding failed!");
        }

        if (listen(descriptor_, 3) < 0) {
            descriptor_ = INVALID_SOCKET;
            throw socket_exception("Starting socket listening failed!");
        }
    }

    void ServerSocket::close_connection() {
        if (descriptor_ != INVALID_SOCKET) {
            for (auto &s : sockets_) {
                ::close(s);
            }
            ::close(descriptor_);
            descriptor_ = INVALID_SOCKET;
        }
    }

    std::shared_ptr<ClientSocket> ServerSocket::accept() {
        int address_len = sizeof(address_);
        int res = ::accept(descriptor_, reinterpret_cast<struct sockaddr *>(&address_),
                           reinterpret_cast<socklen_t *>(&address_len));
        if (res != INVALID_SOCKET) {
            sockets_.insert(res);
        }
        return std::make_shared<ClientSocket>(res);
    }

    ServerSocket::~ServerSocket() {
        close_connection();
    }

} // namespace network