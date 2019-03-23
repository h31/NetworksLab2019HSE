#ifndef ROULETTE_CLIENT_H
#define ROULETTE_CLIENT_H


#include <cstdint>
#include <../include/message.h>
#include <shared_mutex>
#include <map>
#include <thread>
#include <boost/thread/sync_queue.hpp>

class RouletteClient {
public:

    bool StartClient(const char *host, uint16_t port_number);

private:

    int sockfd_;

    boost::sync_queue<Message> requests_;
    boost::sync_queue<Message> responses_;

    std::thread request_sender_;
    std::thread response_receiver_;

    std::mutex io_mutex_;

public:
    void AuthorisePlayer(const std::string &name);

    void AuthoriseCroupier(const std::string &key);

    void StartDraw();

    void FinishDraw();

    void NewBet(const std::string &type, int sum);

    void HandleIncorrectMessage(const Message &response);

    void HandleUnauthorised();

    void PrintResults(const Message &response);

    void HandleUnexpectedServerResponse(const Message &response);

    void HandleResponse(const Message &response);

    void ListBets();

    void Quit();

    void PrintHeader(const std::string &header);

    void PrintPrompt();

    void Cout(const std::string &message);

    bool GetLine(std::string &message);

};


#endif //ROULETTE_CLIENT_H
