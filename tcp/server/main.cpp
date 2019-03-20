#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <stdexcept>
#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>


std::set<std::string> client_names;

struct client_header {
    uint32_t type;
    uint32_t username_length;
    char * username;
};

class client {
private:
    int fd_;
    std::string inbuf;
    std::string outbuf;

    // can be moved out to a separate object
    void parse();
    bool verify_header();
    void handle_read_data();
    void handle_registration();

    void answer_registration(bool answer);

    client_header * current_client_header_ = nullptr;

    uint32_t username_length;
    char * username;

public:
    explicit client(int fd) : fd_(fd) {}

    int fd() const { return fd_; }

    void close() {
        ::close(fd_);
        fd_ = -1;
    }

    bool is_alive() const { return fd_ != -1; }

    bool has_outbuf() const { return !outbuf.empty(); }

    void read();

    void write();

    bool get_line(std::string &str);

    void push_line(const std::string &str);
};

int g_listen_fd = -1;
std::vector<std::unique_ptr<client>> g_clients;

int _check(int ret, const char *msg) {
    if (ret != -1)
        return ret;

    std::string strmsg(msg);
    strmsg.erase(strmsg.find('('));
    throw std::runtime_error(strmsg + "() failed: " + strerror(errno));
}

#define check(x) (_check(x, #x))

int make_socket(int port) {
    int sock = check(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    check(bind(sock, (const struct sockaddr *) &addr, sizeof(addr)));

    check(listen(sock, 16));
    return sock;
}

void client::read() {
    char buf[4096];
    ssize_t len = ::recv(fd(), buf, sizeof(buf), 0);
    if (len == -1) {
        close();
        return;
    }
    inbuf += std::string(buf, buf + len);

    handle_read_data();
}

void client::handle_read_data() {
    parse();
    if (current_client_header_->type == 0) {
        handle_registration();
    }
}

void client::handle_registration() {
    if (!verify_header()) {
        return;
    }

    std::string username = std::string(current_client_header_->username,
        current_client_header_->username_length);
    bool answer = false;
    if (!client_names.count(username)) {
        client_names.insert(username);
//        this->username = username.data();
        answer = true;
    }
    answer_registration(answer);
}

std::string uint32t_to_string(uint32_t uint) {
    char * chars = new char[4];
    for (int i = 0; i < 4; i++) {
        chars[i] = *((char *)(&uint) + i);
    }
    return std::string(chars, 4);
}

void client::answer_registration(bool answer) {
    if (answer) {
        outbuf += uint32t_to_string(0);
    } else {
        outbuf += uint32t_to_string(619);
    }
}

client_header * parse_header(const std::string & str) {
    auto header = new client_header();
    if (str.size() < sizeof(uint32_t) * 2) {
        return nullptr;
    }
    header->type = *((uint32_t *)str.substr(sizeof(uint32_t)).data());
    header->username_length = *((uint32_t *)str.substr(sizeof(uint32_t), sizeof(uint32_t)).data());
    if (str.size() < sizeof(uint32_t) * 2 + header->username_length) {
        return nullptr;
    }
    header->username = new char[header->username_length];
    memcpy(header->username,
            str.substr(sizeof(uint32_t) * 2, header->username_length).data(),
            header->username_length);
    return header;
}

//void parse_body(uint32_t type, const std::string & str) {
//    if (str.size() < sizeof(uint32_t)) {
//        return nullptr;
//    }
//}

bool client::verify_header() {
    if (current_client_header_ == nullptr) {
        return false;
    }
    if (current_client_header_->type < 0 ||
            current_client_header_->type > 3) {
        std::cerr << "Operation type is not correct: " <<
            current_client_header_->type << std::endl;
        return false;
    }
    if (current_client_header_->username_length != username_length) {
        std::cerr << "Verification (username length) do not match" << std::endl;
        return false;
    }
    for (int i = 0; i < username_length; i++) {
        if (current_client_header_->username[i] != username[i]) {
            std::cerr << "Verification (username) do not match" << std::endl;
            return false;
        }
    }
    return true;
}

void client::parse() {
    if (current_client_header_ == nullptr) {
        current_client_header_ = parse_header(inbuf);
        if (current_client_header_ != nullptr) {
            inbuf.erase(0,
                    sizeof(uint32_t) * 2 + current_client_header_->username_length);
        }
    }
//    if (current_client_header_ != nullptr && verify_header()) {
//        parse_body(current_client_header_->type, inbuf);
//    }
}

void client::write() {
    ssize_t len = ::send(fd(), outbuf.data(), outbuf.size(), 0);
    if (len == -1)
        close();
    outbuf.erase(0, (unsigned long) len);
}

bool client::get_line(std::string &str) {
    size_t newline = inbuf.find('\n');
    if (newline == std::string::npos)
        return false;

    str = inbuf.substr(0, newline);
    inbuf.erase(0, newline + 1);
    return true;
}

void client::push_line(const std::string &str) {
    outbuf += (str + "\n");
}

void loop() {
    fd_set rfds, wfds;
    int max_fd = g_listen_fd;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_SET(g_listen_fd, &rfds);
    for (const auto &cl: g_clients) {
        max_fd = std::max(max_fd, cl->fd());
        FD_SET(cl->fd(), &rfds);
        if (cl->has_outbuf())
            FD_SET(cl->fd(), &wfds);
    }

    check(select(max_fd + 1, &rfds, &wfds, nullptr, nullptr));

    if (FD_ISSET(g_listen_fd, &rfds)) {
        int fd = accept(g_listen_fd, nullptr, nullptr);
        g_clients.push_back(std::make_unique<client>(fd));
    }

    for (auto &cl: g_clients)
        if (FD_ISSET(cl->fd(), &rfds))
            cl->read();


//    std::string line;
//    for (auto &from: g_clients)
//        if (from->get_line(line))
//            for (auto &to: g_clients)
//                if (from.get() != to.get())
//                    to->push_line(line);

    for (auto &cl: g_clients)
        if (cl->has_outbuf() && FD_ISSET(cl->fd(), &wfds))
            cl->write();

//    g_clients.erase(
//            std::remove_if(
//                    g_clients.begin(), g_clients.end(),
//                    [](std::unique_ptr<client> &cl) { return !cl->is_alive(); }),
//            g_clients.end());
}

int main(int argc, char **argv) {
    int port = 80;
    if (argc >= 2)
        port = atoi(argv[1]);

    g_listen_fd = make_socket(port);
    while (true) {
        try {
            loop();
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }
}
