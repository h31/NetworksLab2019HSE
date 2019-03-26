#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <string>
#include <netdb.h>
#include <zconf.h>
#include <vector>
#include <fcntl.h>


int fd_set_blocking(int fd, int blocking) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return 0;

    if (blocking)
        flags &= ~O_NONBLOCK;
    else
        flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags) != -1;
}


class client {
private:
    int client_id_;
    int sockfd_;

    void write_header(int operation_type) {
        write(sockfd_, (char *) &operation_type, sizeof(operation_type));
        write(sockfd_, (char *) &(this->client_id_), sizeof(this->client_id_));
    }

    uint32_t read_uint32_t() {
        uint32_t num;
        read(sockfd_, (char *) & num, sizeof(num));
        return num;
    }

    std::string read_uint32_t_and_convert_to_string() {
        return std::to_string(read_uint32_t());
    }

    std::string read_create_order_response() {
        return read_uint32_t_and_convert_to_string();
    }

    std::string read_retrieve_history_response() {
        uint32_t num_orders = read_uint32_t();
        std::string response = "Number of orders: " + std::to_string(num_orders) + "\n";
        for (int i = 0; i < num_orders; i++) {
            uint32_t client_id = read_uint32_t();
            uint32_t shop_id = read_uint32_t();
            uint32_t num_goods = read_uint32_t();

            response += "Order #" + std::to_string(i) + ":\n";
            response += "Client ID: " + std::to_string(client_id) + ":\n";
            response += "Shop ID: " + std::to_string(shop_id) + ":\n";
            response += "Num goods: " + std::to_string(num_goods) + ":\n";
            response += "Goods numbers: ";
            for (int j = 0; j < num_goods; j++) {
                uint32_t good_id = read_uint32_t();
                response += std::to_string(good_id);
            }
            response += "\n\n";
        }
        return response;
    }

    std::string read_add_shop_response() {
        return read_uint32_t_and_convert_to_string();
    }

    std::string read_disconnect_response() {
        return "";
    }
public:
    client(const std::string & hostname, int port, int client_id) : client_id_(client_id) {
        struct hostent * host = gethostbyname(hostname.data());
        struct sockaddr_in serv_addr{};
        // Create socket.
        sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd_ < 0) {
            perror("ERROR opening socket");
            exit(1);
        }
        // Validate host
        if (host == nullptr) {
            fprintf(stderr, "ERROR, no such host\n");
            exit(0);
        }
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy(host->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) host->h_length);
        serv_addr.sin_port = htons(port);
    }

    std::string receive_incoming_event() {
        fd_set_blocking(sockfd_, true);
        int32_t type = 0;
        ssize_t n = read(sockfd_, (char *) &type, sizeof(type));
        if (n == 0)
            return {};
        fd_set_blocking(sockfd_, false);

        switch (type) {
            case 1: {
                return read_create_order_response();
            }
            case 2: {
                return read_retrieve_history_response();
            }
            case 3: {
                return read_add_shop_response();
            }
            case 4: {
                return read_disconnect_response();
            }
            default: {
                return "Unknown response from the server.";
            }
        }
    }

    void create_order(int shop_id, const std::vector<int> & goods_ids) {
        const int operation_type = 1;
        this->write_header(operation_type);
        write(sockfd_, (char *) & shop_id, sizeof(shop_id));
        int num_goods = goods_ids.size();
        write(sockfd_, (char *) & num_goods, sizeof(num_goods));
        for (int good_id : goods_ids) {
            write(sockfd_, (char *) & good_id, sizeof(good_id));
        }
    }

    void retrieve_order_history() {
        const int operation_type = 2;
        this->write_header(operation_type);
    }

    void add_new_shop(const std::vector<int> & goods_ids, int zone_id) {
        const int operation_type = 3;
        this->write_header(operation_type);
        int num_goods = goods_ids.size();
        write(sockfd_, (char *) & num_goods, sizeof(num_goods));
        for (int good_id : goods_ids) {
            write(sockfd_, (char *) & good_id, sizeof(good_id));
        }
        write(sockfd_, (char *) & zone_id, sizeof(zone_id));
    }

    void disconnect() {
        const int operation_type = 4;
        this->write_header(operation_type);
    }
};


#endif //CLIENT_CLIENT_H
