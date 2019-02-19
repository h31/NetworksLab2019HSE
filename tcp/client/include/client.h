#ifndef ROULETTE_CLIENT_H
#define ROULETTE_CLIENT_H


#include <cstdint>
#include <shared_mutex>
#include <map>
#include <thread>
#include <boost/thread/sync_queue.hpp>
#include <../include/message.h>

class MarketClient {
public:

    bool StartClient(const char *host, uint16_t port_number);

private:

    int sockfd_;

    std::mutex io_mutex_;

    Message SendMessage(Message::Type type);

    Message SendMessage(Message::Type type, const std::string& text);

public:
    bool AuthoriseCustomer(const std::string& name);

    bool AuthoriseFreelancer(const std::string& name);

    void ListMyOrders();

    void ListOpenOrders();

    void NewOrder(const std::string& description);

    void RequestOrder(int order_id);

    void StartOrder(int order_id);

    void FinishOrder(int order_id);

    void GiveOrder(int order_id, const std::string& name);

    void ApproveDoneOrder(int order_id);

    void HandleIncorrectMessage(const Message &response);

    void HandleUnauthorised();

    void HandleUnexpectedServerResponse(const Message &response);

    void HandleResponse(const Message &response);

    void Quit();

    void PrintHeader(const std::string &header);

    void PrintPrompt();

    void Cout(const std::string &message);

    bool GetLine(std::string &message);

};


#endif //ROULETTE_CLIENT_H
