#include <util/user_view.h>

namespace util {

    const std::string ClientCommands::commands_names[] = {"send", "check", "get", "help", "exit"};
    const std::string ClientCommands::commands_args[] = {"<recipient> <theme> <body>", "", "<id>", "help", "exit"};
    const std::string ClientCommands::commands_actions[] = {
        "send mail",
        "check new messages",
        "get message with appropriate id",
        "print this message",
        "shut down the client"
    };

    std::string UserView::get_user_input(const std::string &str) {
        std::string input;
        if (!str.empty()) {
            std::cout << str << std::endl;
        }
        std::cout << "> ";
        if (getline(std::cin, input)) {
            return input;
        }
        return "exit";
    }

    void UserView::println(const std::string &str) {
        std::cout << str << std::endl;
    }

    void UserView::println(const email::Email &e) {
        std::cout << "author:\t" << e.get_author() << std::endl;
        std::cout << "recipient:\t" << e.get_recipient() << std::endl;
        std::cout << "theme:\t" << e.get_theme() << std::endl;
        std::cout << "---------------------" << std::endl;
        std::cout << e.get_body() << std::endl;
    }

    void UserView::println(const email::EmailInfo &i) {
        std::cout << "id:\t" << i.get_id() << std::endl;
        std::cout << "author:\t" << i.get_author() << std::endl;
        std::cout << "theme:\t" << i.get_theme() << std::endl;
    }

} // namespace util