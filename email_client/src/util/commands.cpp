#include "util/commands.h"

namespace util {

    const std::string ClientCommands::commands_names[] = {
        "send", "check", "get", "help", "exit"
    };

    const std::string ClientCommands::commands_args[] = {
        "<recipient> <theme> <body>", "", "<id>", "help", "exit"
    };

    const std::string ClientCommands::commands_actions[] = {
        "send mail",
        "check new messages",
        "get message with appropriate id",
        "print this message",
        "shut down the client"
    };

} // namespace util