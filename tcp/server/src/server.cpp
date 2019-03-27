#include "server.h"
#include "socket_io.h"
#include "socket_opening_exception.h"
#include "binding_exception.h"

server::server(uint16_t port) : port_number(port) {}

server::~server() {
    stop();
}

void server::start() {
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd < 0) {
        throw socket_opening_exception();
    }

    sockaddr_in server_address{};
    bzero((char*)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port_number);

    if (bind(server_socket_fd, (sockaddr*)&server_address, sizeof(server_address)) < 0) {
        throw binding_exception();
    }
    listen(server_socket_fd, CONNECTION_QUEUE_SIZE);

    main_thread = new boost::thread(&server::client_accept_cycle, this, server_socket_fd);
}

void server::stop() {
    if (server_socket_fd < 0) {
        return;
    }

    shutdown(server_socket_fd, SHUT_RDWR);
    server_socket_fd = -1;
    main_thread->join();
    for (auto& client : clients) {
        client->join();
    }

    for (auto& client : clients) {
        delete client;
    }

    delete main_thread;
}

void server::client_accept_cycle(int server_socket_fd) {
    while (true) {
        sockaddr_in client_address{};
        unsigned int client_length = sizeof(client_address);
        int client_socket_fd = accept(server_socket_fd, (sockaddr*)&client_address, &client_length);
        if (client_socket_fd >= 0) {
            auto client_thread = new boost::thread(&server::request_response_cycle, this, client_socket_fd);
            clients.push_back(client_thread);
            client_sockets.push_back(client_socket_fd);
        } else {
            for (int client_socket : client_sockets) {
                shutdown(client_socket, SHUT_RDWR);
                close(client_socket);
            }
            break;
        }
    }
}

void server::request_response_cycle(int client_socket_fd) {
    auto io = socket_io(client_socket_fd);

    try {
        while (true) {
            auto command = io.read_data<int32_t>();
            switch (command) {
                case 1:
                    output_topics_list(io);
                    break;
                case 2:
                    output_news_by_topic(io);
                    break;
                case 3:
                    output_news_content(io);
                    break;
                case 4:
                    add_news(io);
                    break;
                default:
                    break;
            }
        }
    } catch (...) {
        close(client_socket_fd);
    }
}

void server::output_topics_list(socket_io &io) {
    boost::shared_lock<boost::shared_mutex> lock(news_access);

    std::set<std::string> topics;
    for (auto& news : news_list) {
        topics.insert(news.topic);
    }

    io.write_data<uint64_t>(topics.size());
    for (auto& topic : topics) {
        io.write_data<uint64_t>(topic.size());
        io.write_data(topic);
    }
}

void server::output_news_by_topic(socket_io& io) {
    auto topic_length = io.read_data<uint64_t>();
    std::string topic = io.read_string(topic_length);

    boost::shared_lock<boost::shared_mutex> lock(news_access);

    std::vector<int> news_by_topic_ids;
    for (int i = 0; i < news_list.size(); ++i) {
        if (news_list[i].topic == topic) {
            news_by_topic_ids.push_back(i);
        }
    }

    io.write_data<uint64_t>(news_by_topic_ids.size());
    for (int id : news_by_topic_ids) {
        io.write_data<int32_t>(id);
        auto& news_title = news_list[id].title;
        io.write_data<uint64_t>(news_title.size());
        io.write_data(news_title);

    }
}

void server::output_news_content(socket_io &io) {
    auto id = io.read_data<int32_t >();

    boost::shared_lock<boost::shared_mutex> lock(news_access);

    if (id < 0 || id >= news_list.size()) {
        io.write_data<uint64_t>(0);
        return;
    }

    auto& news_content = news_list[id].content;
    io.write_data<uint64_t>(news_content.size());
    io.write_data(news_content);
}

void server::add_news(socket_io &io) {
    auto topic_length = io.read_data<uint64_t>();
    std::string topic = io.read_string(topic_length);
    auto news_title_length = io.read_data<uint64_t>();
    std::string news_title = io.read_string(news_title_length);
    auto news_content_length = io.read_data<uint64_t>();
    std::string news_content = io.read_string(news_content_length);

    boost::upgrade_lock<boost::shared_mutex> lock(news_access);
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
    news_list.emplace_back(topic, news_title, news_content);
}
