#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <string>
#include <vector>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <boost/thread.hpp>

#include "news.h"
#include "socket_io.h"


class server {
    private:
        static const int CONNECTION_QUEUE_SIZE = 5;
        const uint16_t port_number;
        int server_socket_fd;
        boost::thread *main_thread = nullptr;
        std::vector<boost::thread*> clients;
        std::vector<int> client_sockets;
        std::vector<news> news_list;
        boost::shared_mutex news_access;

        void client_accept_cycle(int server_socket_fd);
        void request_response_cycle(int client_socket_fd);
        void output_topics_list(socket_io& io);
        void output_news_by_topic(socket_io& io);
        void output_news_content(socket_io& io);
        void add_news(socket_io& io);

    public:
        explicit server(uint16_t port);
        ~server();
        void start();
        void stop();
};

#endif //SERVER_SERVER_H
