#include <cstdint>
#include <string.h>
#include <string>
#include <netdb.h>
#include "../include/client.h"
#include "../include/query_type.h"
#include "../include/network_exception.h"


client::client(const std::string &host, uint16_t port) {
    struct hostent *server = gethostbyname(host.c_str());
    if (server == nullptr) {
        throw network_exception("No such host");
    }

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        throw network_exception("Unable to open socket");
    }
    io = new socket_io(socket_fd);

    struct sockaddr_in server_address{};
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy(server->h_addr, &server_address.sin_addr.s_addr, (size_t) server->h_length);
    server_address.sin_port = htons(port);

    if (connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        throw network_exception("Unable to connect to the server");
    }
}

client::~client() {
    io->close();
    delete io;
}

void client::add_news(const std::string &topic, const std::string &title, const std::string &text) {
    io->write<int32_t>(static_cast<int>(ADD_NEWS));
    io->write<uint64_t>(topic.length());
    io->write(topic);
    io->write<uint64_t>(title.length());
    io->write(title);
    io->write<uint64_t>(text.length());
    io->write(text);
}

std::string client::get_news_text(int news_id) {
    io->write<int32_t>(static_cast<int>(GET_NEWS_CONTENT));
    io->write<int32_t>(news_id);
    auto news_text_length = io->read<uint64_t >();
    if (news_text_length == 0) {
        return "There is no news with this id.";
    }
    std::string text = io->read_string(news_text_length);
    return text;
}

std::vector<client::news_title> client::list_news(const std::string &topic) {
    io->write<int32_t>(static_cast<int>(LIST_NEWS));
    io->write<uint64_t>(topic.length());
    io->write(topic);

    auto news_number = io->read<uint64_t>();
    std::vector<news_title> news_titles;
    for (int i = 0; i < news_number; i++) {
        int news_id = io->read<int32_t>();
        auto news_title_length = io->read<uint64_t>();
        std::string title = io->read_string(news_title_length);
        news_titles.emplace_back(news_id, title);
    }

    return news_titles;
}

std::vector<std::string> client::list_topics() {
    io->write<int32_t>(static_cast<int>(LIST_TOPICS));
    auto topics_number = io->read<uint64_t>();

    std::vector<std::string> topics;
    for (int i = 0; i < topics_number; i++) {
        auto topic_length = io->read<uint64_t>();
        topics.push_back(io->read_string(topic_length));
    }

    return topics;
}

client::news_title::news_title(int id, const std::string &title) : id(id), title(title) {}
