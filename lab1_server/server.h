#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string.h>

#include <pthread.h>

#include <set>
#include <vector>

using std::set;
using std::vector;
using std::max;
using std::min;

typedef long long calc_t;

struct segment {
    segment(calc_t left, calc_t right);
    calc_t left;
    calc_t right;
    bool operator < (struct segment other);
};

class Server {
private:
    int server_sockfd;
    calc_t max_val;
    vector<calc_t> calculated;
    set<segment> unprocessed;
    pthread_mutex_t mutex;
public:
    Server(int portno);
    ~Server();
    calc_t get_max();
    vector<calc_t> get_last_n(size_t n);
    segment ask_to_calculate(size_t n);
    void add_calculated(vector<calc_t> &calculated);
    void run();
};

struct task_args {
    Server* server;
    int sockfd;
    task_args(Server* server, int sockfd);
};