#include <cstdint>
#include <string.h>
#include <string>
#include "../include/client.h"
#include "../include/network_exception.h"


client::client(const std::string &host, uint16_t port) {
    struct hostent *server = gethostbyname(host.c_str());
    if (server == nullptr) {
        throw network_exception("No such host exception");
    }

    struct sockaddr_in server_address{};
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy(server->h_addr, &server_address.sin_addr.s_addr, (size_t) server->h_length);
    server_address.sin_port = htons(port);

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        throw network_exception("Unable to open socket");
    }
    if (connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        throw network_exception("Unable to connect to the server");
    }
    io = new socket_io(socket_fd);
}

client::~client() {
    io->close();
    delete io;
}

void client::add_news(const std::string &topic, const std::string &title, const std::string &text) {
    io->write_int(4);
    io->write_size_t(topic.length());
    io->write_string(topic);
    io->write_size_t(title.length());
    io->write_string(title);
    io->write_size_t(text.length());
    io->write_string(text);
}

std::string client::get_news_text(int news_id) {
    io->write_int(3);
    io->write_int(news_id);
    size_t news_text_length = io->read_size_t();
    std::string text = io->read_string(news_text_length);
    return text;
}

std::vector<client::news_title> client::list_news(const std::string &topic) {
    io->write_int(2);
    io->write_size_t(topic.length());
    io->write_string(topic);

    size_t news_number = io->read_size_t();
    std::vector<news_title> news_titles;
    for (int i = 0; i < news_number; i++) {
        int news_id = io->read_int();
        size_t news_title_length = io->read_size_t();
        std::string title = io->read_string(news_title_length);
        news_titles.emplace_back(news_id, title);
    }

    return news_titles;
}

std::vector<std::string> client::list_topics() {
    io->write_int(1);
    size_t topics_number = io->read_size_t();

    std::vector<std::string> topics(topics_number);
    for (int i = 0; i < topics_number; i++) {
        size_t topic_length = io->read_size_t();
        topics.push_back(io->read_string(topic_length));
    }

    return topics;
}

client::news_title::news_title(int id, const std::string &title) : id(id), title(title) {}
