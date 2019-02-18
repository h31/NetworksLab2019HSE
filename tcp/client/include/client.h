#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <vector>
#include "socket_io.h"


class client {
    private:
        socket_io *io;

    public:
        explicit client(const std::string &host, uint16_t port);

        ~client();

        std::vector<std::string> list_topics();

        struct news_title {
                int id;
                std::string title;

                news_title(int id, const std::string &title);
        };

        std::vector<news_title> list_news(const std::string &topic);

        std::string get_news_text(int news_id);

        void add_news(const std::string &topic, const std::string &title, const std::string &text);
};

#endif //CLIENT_CLIENT_H
