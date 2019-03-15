#pragma once

#include "pch.h"
#include "email.h"
#include "network/response.h"
#include "network/request.h"
#include "network/client_socket.h"
#include "util/user_view.h"

namespace email {

    using port_t = uint16_t;
    using net_address_t = std::string;

    class Client {
    public:
        static const port_t DEFAULT_SERVER_PORT;
        static const std::string DEFAULT_SERVER_ADDRESS;

        Client(const std::string &host_email, const std::string &server_address, uint16_t port);

        static Client from_input();

        void run();

        void shut_down();

    private:

        const std::string host_email_;
        const std::string server_address_;
        const port_t server_port_;
        bool is_alive_;
        network::ClientSocket socket_;

        void send_email(const Email &email);

        void check_email();

        void get_email(uint32_t id);

        void print_help();

        void process_error_response(const std::shared_ptr<response::Response>& response);
    };

} // namespace email