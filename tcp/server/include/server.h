#include <utility>

#include <utility>

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "../../message/message.h"

#include <thread>
#include <mutex>
#include <vector>
#include <map>

#include <csignal>
#include <cstdint>

#include <netdb.h>
#include <signal.h>
#include <netinet/in.h>
#include <unistd.h>
#include <queue>


class InternalMessage {
public:
    InternalMessage(int receiver, ResponseMessage response_message):
        receiver_(receiver), response_message_(std::move(response_message)) {};
    ResponseMessage GetResponseMessage();
    int GetReceiver();
private:
    int receiver_;
    ResponseMessage response_message_;
};

class Server
{
public:
    explicit Server(uint16_t port);

    ~Server();

    void Run();

    void Stop();

    void SignalStop(int signum);

private:
    uint16_t port_;

    int sockfd_;

    sockaddr_in serv_addr_;

    sockaddr_in cli_addr_;

    int last_client_id_ = 1;

    std::thread sender_thread_;

    std::vector<std::thread> client_threads_;

    std::map<int, int> client_sockets_;

    std::mutex clients_lock;

    std::queue<InternalMessage> message_queue_;

    std::mutex queue_lock;

    volatile std::sig_atomic_t exit_ = 0;

    void SendSuccessMessage(int id);

    void ClientLifeCycle(int newsockfd, int id);

    void SenderLifeCycle();

    void SendMessage(int sender, RequestMessage message);
};
#endif // TCP_SERVER_H
