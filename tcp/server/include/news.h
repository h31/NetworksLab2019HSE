#ifndef SERVER_NEWS_H
#define SERVER_NEWS_H

#include <string>


struct news {
public:
    std::string topic;
    std::string title;
    std::string content;

    news(std::string topic, std::string title, std::string content) :
            topic(std::move(topic)), title(std::move(title)), content(std::move(content)) {}
};

#endif //SERVER_NEWS_H
