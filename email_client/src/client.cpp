#include <client.h>

namespace email {

    using namespace util;

    const port_t Client::DEFAULT_SERVER_PORT = 1111;
    const std::string Client::DEFAULT_SERVER_ADDRESS = "127.0.0.1";

    Client::Client(const std::string &host_email,
                   const std::string &server_address = DEFAULT_SERVER_ADDRESS,
                   port_t port = DEFAULT_SERVER_PORT
    )
        : host_email_(host_email),
          server_address_(server_address),
          server_port_(port),
          is_alive_(false),
          socket_(network::ClientSocket(server_address_, port)) {}

    Client Client::from_input() {
        std::string email = util::UserView::get_user_input("Enter your email:");

        std::string server = email::Client::DEFAULT_SERVER_ADDRESS;
        std::string server_str = util::UserView::get_user_input("Enter server address or leave default 127.0.0.1:");
        server = server_str.empty() ? server : server_str;

        uint16_t port = email::Client::DEFAULT_SERVER_PORT;
        std::string port_str = util::UserView::get_user_input(
            "Enter server port or leave default " + std::to_string(port) + ":");
        port = port_str.empty() ? port : static_cast<uint16_t>(std::stoi(port_str));
        return {email, server, port};
    }

    void Client::run() {
        is_alive_ = true;

        while (is_alive_) {
            ClientCommands::CommandID cmd = ClientCommands::str_to_id(UserView::get_user_input());
            switch (cmd) {
                case ClientCommands::SEND: {
                    std::string recipient = UserView::get_user_input("Enter the recipient email: ");
                    if (recipient == host_email_) {
                        UserView::println("Invalid recipient");
                        break;
                    }
                    std::string theme = UserView::get_user_input("Enter the email theme: ");
                    std::string body = UserView::get_user_input("Enter the email body: ");
                    Email email(recipient, host_email_, theme, body);
                    send_email(email);
                    break;
                }
                case ClientCommands::CHECK: {
                    check_email();
                    break;
                }
                case ClientCommands::GET: {
                    std::string input = UserView::get_user_input("Enter the email id:");
                    auto email_id = static_cast<uint32_t>(std::atoi(input.data()));
                    get_email(email_id);
                    break;
                }
                case ClientCommands::EXIT: {
                    shut_down();
                    break;
                }
                default: {
                    print_help();
                    break;
                }
            }
        }
    }

    void Client::shut_down() {
        socket_.close_connection();
        is_alive_ = false;
    }

    void Client::print_help() {
        UserView::println("Available commands for email client:");
        for (size_t cmd = 0; cmd < ClientCommands::COMMANDS_NUM - 1; ++cmd) {
            auto id = static_cast<ClientCommands::CommandID>(cmd);
            UserView::println(ClientCommands::info_string(id));
        }
    }

    void Client::send_email(const Email &email) {
        std::unique_ptr<request::Request> request(new request::SendRequest(email));
        std::shared_ptr<response::Response> response = socket_.send_request(request);
        if (response->is_error()) {
            process_error_response(response);
            return;
        }
        util::UserView::println("Message sent");
    }

    void Client::check_email() {
        std::unique_ptr<request::Request> request(new request::CheckRequest(host_email_));
        std::shared_ptr<response::Response> response = socket_.send_request(request);

        if (response->is_error()) {
            process_error_response(response);
            return;
        }

        auto check_response = reinterpret_cast<response::CheckResponse *>(response.get());
        auto infos = check_response->get_infos();
        util::UserView::println(infos.empty() ? "No new messages:" : "New messages:");
        for (auto &info : infos) {
            util::UserView::println(info);
        }
    }

    void Client::get_email(uint32_t id) {
        std::unique_ptr<request::Request> request(new request::GetRequest(host_email_, id));
        std::shared_ptr<response::Response> response = socket_.send_request(request);

        if (response->is_error()) {
            process_error_response(response);
            return;
        }

        auto get_response = reinterpret_cast<response::GetResponse *>(response.get());
        UserView::println(get_response->get_email());
    }

    void Client::process_error_response(const std::shared_ptr<response::Response> &response) {
        auto error_response = reinterpret_cast<response::BadResponse *>(response.get());
        UserView::println("Some errors were acquired:");
        UserView::println(error_response->get_error_message());
    }

} // namespace email
