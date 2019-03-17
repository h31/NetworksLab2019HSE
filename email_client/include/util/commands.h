#pragma once

#include "pch.h"

namespace util {

    struct ClientCommands {

        static const size_t COMMANDS_NUM = 6;

        enum CommandID {
            SEND,
            CHECK,
            GET,
            HELP,
            EXIT,
            NONE
        };

        static const std::string commands_names[COMMANDS_NUM];
        static const std::string commands_args[COMMANDS_NUM];
        static const std::string commands_actions[COMMANDS_NUM];

        static CommandID str_to_id(const std::string &str) {
            if (str == "send") return SEND;
            if (str == "check") return CHECK;
            if (str == "get") return GET;
            if (str == "help") return HELP;
            if (str == "exit") return EXIT;
            return NONE;
        }

        static std::string name(CommandID id) {
            return commands_names[id];
        }

        static std::string args(CommandID id) {
            return commands_args[id];
        }

        static std::string description(CommandID id) {
            return commands_actions[id];
        }

        static std::string info_string(CommandID id) {
            return name(id) + " " +  args(id) + " - " + description(id);
        }
    };

} // namespace util