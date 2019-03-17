#pragma once

#include "pch.h"
#include "client_socket.h"
#include "socket_exception.h"

namespace network {

    class ServerSocket {

    public:

        explicit ServerSocket(uint16_t port);

        ~ServerSocket();

        void open_connection();

        std::shared_ptr<ClientSocket> accept();

        void close_connection();

    private:
        int descriptor_;
        const uint16_t port_;
        struct sockaddr_in address_;
        std::set<int> sockets_;

    };

} // namespace network