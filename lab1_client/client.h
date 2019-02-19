#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#include <string.h>

#include <pthread.h>
#include <signal.h>

#include <string>
#include <stdexcept>
#include <set>
#include <vector>
#include <iostream>
#include <map>

using std::string;
using std::set;
using std::vector;
using std::max;
using std::min;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::runtime_error;
using std::map;

typedef unsigned long long fact_t;
typedef unsigned long long rw_t;
typedef double calc_t;

enum type { SQRT, FACT };

class Client {
public:
    Client(string host, int portno);
    ~Client();
    void run();
    pthread_mutex_t *get_mutex();
    type get_long_query(rw_t id);
private:
    int sockfd;
    pthread_mutex_t mutex;
    map<rw_t, type> long_queries;
};

struct rw_exception : runtime_error {
    rw_exception(const string &what);
};

struct task_args {
    task_args(Client *client, int sockfd);
    Client *client;
    int sockfd;
};