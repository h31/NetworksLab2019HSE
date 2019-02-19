#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <string>
#include <csignal>
#include <thread>

class Client {
    public:
        Client(const std::string& host, uint16_t port): host_(host), port_(port) {}

        void Start();

        void Stop();

        void SendMessageToAll(const std::string& message) const;

        void SendMessageToOne(int32_t receiver_id, const std::string& message) const;

        void Disconnect();

        void Connect();

        ~Client();

    private:
        void process_incoming_messages();

        void print_messages() const;

        std::string host_;

        uint16_t port_;

        volatile std::sig_atomic_t is_running_ = 0;

        volatile std::sig_atomic_t stop_ = 0;

        std::thread main_thread_;

        std::thread messages_thread_;

        int sockfd_;
};

#endif  // TCP_CLIENT_H