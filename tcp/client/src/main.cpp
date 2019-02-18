#include <iostream>
#include <exception>
#include <inttypes.h>
#include "../include/client.h"
#include "../include/network_exception.h"


static const std::string LIST_TOPICS_COMMAND = "topics";
static const std::string LIST_NEWS_COMMAND = "news";
static const std::string NEWS_CONTENT_COMMAND = "content";
static const std::string ADD_NEWS_COMMAND = "add";
static const std::string EXIT_COMMAND = "exit";

void print_usage() {
    std::cout << "Usage:\n" << LIST_TOPICS_COMMAND << " - list all topics\n"
              << LIST_NEWS_COMMAND << " - list all news on specified topic\n"
              << NEWS_CONTENT_COMMAND << " - get news text\n"
              << ADD_NEWS_COMMAND << " - add news\n"
              << EXIT_COMMAND << " - stop application\n" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Host and port should be specified.";
        return 1;
    }

    char *end;
    errno = 0;
    intmax_t port = strtoimax(argv[2], &end, 10);
    if (errno == ERANGE || port < 0 || port > UINT16_MAX || end == argv[1] || *end != '\0') {
        std::cout << "Malformed port.";
        return 1;
    }

    try {
        client client(argv[1], (uint16_t) port);
        print_usage();

        while (true) {
            std::string command;
            std::getline(std::cin, command);
            if (command == LIST_TOPICS_COMMAND) {
                std::vector<std::string> topics = client.list_topics();
                for (const auto &topic : topics) {
                    std::cout << topic << "\n" ;
                }
                std::cout << std::endl;
            } else if (command == LIST_NEWS_COMMAND) {
                std::cout << "Enter topic:" << std::endl;
                std::string topic;
                std::getline(std::cin, topic);
                for (const auto &news_title : client.list_news(topic)) {
                    std::cout << "id: " << news_title.id << "\ntitle: " << news_title.title << "\n" << std::endl;
                }
            } else if (command == NEWS_CONTENT_COMMAND) {
                std::cout << "Enter id:" << std::endl;
                std::string id;
                std::getline(std::cin, id);
                try {
                    std::cout << client.get_news_text(std::stoi(id)) << "\n" << std::endl;
                } catch (std::invalid_argument &e) {
                    std::cout << "id must be an integer\n" << std::endl;
                }
            } else if (command == ADD_NEWS_COMMAND) {
                std::cout << "Enter news topic:" << std::endl;
                std::string topic;
                std::getline(std::cin, topic);
                std::cout << "Enter news title:" << std::endl;
                std::string title;
                std::getline(std::cin, title);
                std::cout << "Enter news text:" << std::endl;
                std::string text;
                std::getline(std::cin, text);
                client.add_news(topic, title, text);
                std::cout << std::endl;
            } else if (command == EXIT_COMMAND) {
                return 0;
            } else {
                std::cout << "Unknown command\n";
                print_usage();
            }
        }
    } catch (network_exception &exception) {
        std::cout << exception.what();
        return 1;
    }
}