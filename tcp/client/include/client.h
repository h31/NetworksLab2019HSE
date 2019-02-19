#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <string>
#include <csignal>
#include <thread>
#include <mutex>

class Client {
    public:
        Client(const std::string& host, uint16_t port): host_(host), port_(port) {}

        void Start();

        void Stop();

        void SendMessageToAll(const std::string& message);

        void SendMessageToOne(int32_t receiver_id, const std::string& message);

        void Disconnect();

        void Connect();

        ~Client();

    private:
        void process_incoming_messages();

        void print_message(const std::string& message);

        std::string host_;

        uint16_t port_;

        volatile std::sig_atomic_t is_running_ = 0;

        std::mutex writing_lock_;

        std::mutex connection_lock_;

        std::thread main_thread_;
        
        int sockfd_;
};

#endif  // TCP_CLIENT_H