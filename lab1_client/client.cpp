#include "client.h"

Client::Client(string host, int portno) {
    /* Create a socket point */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    struct hostent *server = gethostbyname(host.c_str());
    struct sockaddr_in serv_addr;

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons(portno);

    /* Now connect to the server */
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    pthread_mutex_init(&mutex, NULL);
}

Client::~Client() {
    pthread_mutex_destroy(&mutex);
}

rw_t read_num(int sockfd) {
    rw_t buffer[2];
    bzero(buffer, sizeof(rw_t) * 2);
    ssize_t n;
    n = read(sockfd, buffer, sizeof(rw_t));
    if (n <= 0) {
        throw rw_exception("ERROR reading from socket");
    }
    return buffer[0];
}

void write_nums(int sockfd, vector<rw_t*> nums) {
    rw_t buffer[256];
    ssize_t n;
    for (int i = 0; i < nums.size();) {
        int j = 0;
        for (; j < 255 && i + j < nums.size(); j++) {
            buffer[j] = *nums[i + j];
        }
        n = write(sockfd, buffer, sizeof(rw_t) * j);
        if (n < 0) {
            throw rw_exception("ERROR writing to socket");
        }
        i += j;
    }
}

rw_t get_res(int sockfd, Client* client) {
    rw_t t = read_num(sockfd);
    while (t == 3) {
        rw_t id = read_num(sockfd);
        rw_t res = read_num(sockfd);
        cout << "Result of query " << id << " ";
        type tp = client->get_long_query(id);
        if (tp == SQRT)
            cout << "(get_sqrt) is " << (calc_t) res;
        else if (tp == FACT)
            cout << "(get_fact) is " << (fact_t) res;
        cout << endl;
        t = read_num(sockfd);
    }
    return read_num(sockfd);
}

void *listen(void* args) {
    Client* client = ((task_args*) args)->client;
    int sockfd = ((task_args*) args)->sockfd;
    while (true) {
        pthread_mutex_lock(client->get_mutex());
        rw_t buf[4];
        bzero(buf, sizeof(rw_t) * 4);
        int flags = fcntl(sockfd, F_GETFL, 0);
        fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
        int n = read(sockfd, buf, 3);
        fcntl(sockfd, F_SETFL, flags);
        if (n > 0) {
            while (n < 3) {
                int m = read(sockfd, buf + n, 3 - n / 4);
                n += m;
            }
            rw_t id = buf[1];
            rw_t res = buf[2];
            cout << "Result of query " << id << " ";
            type tp = client->get_long_query(id);
            if (tp == SQRT)
                cout << "(get_sqrt) is " << (calc_t) res;
            else if (tp == FACT)
                cout << "(get_fact) is " << (fact_t) res;
            cout << endl;
        }
        pthread_mutex_unlock(client->get_mutex());
        sleep(5);
    }
}

void Client::run() {
    
    cout << "List of commands\n"
        "List of commands:\n"
        "1. get_sum <a> <b>\n"
        "2. get_mult <a> <b>\n"
        "3. get_diff <a> <b>\n"
        "4. get_quot <a> <b>\n"
        "5. get_sqrt <a>\n"
        "6. get_fact <n>\n"
        "7. exit\n"
    ;

    pthread_t *thread = (pthread_t*) malloc(sizeof(pthread_t));
    pthread_create(thread, NULL, listen, new task_args(this, sockfd));

    string command;
    while (cin >> command) {
        rw_t type;
        if (command == "get_sum") {
            type = 1;
        } else if (command == "get_mult") {
            type = 3;
        } else if (command == "get_diff") {
            type = 2;
        } else if (command == "get_quot") {
            type = 4;
        } else if (command == "get_sqrt") {
            type = 6;
        } else if (command == "get_fact") {
            type = 5;
        } else if (command == "exit") {
            pthread_kill(*thread, 0);
            free(thread);
            close(sockfd);
            return;
        } else {
            cerr << "Incorrect command" << endl;
            continue;
        }
        if (type >= 1 && type <= 4) {
            calc_t a, b;
            cin >> a >> b;
            pthread_mutex_lock(&mutex);
            write_nums(sockfd, {&type, (rw_t*) &a, (rw_t*) &b});
            rw_t store = get_res(sockfd, this);
            calc_t res = *((calc_t*) &store);
            pthread_mutex_unlock(&mutex);
            cout << "Result of query " << command << " is " << res << endl;
        } else if (type == 6) {
            calc_t a;
            cin >> a;
            pthread_mutex_lock(&mutex);
            write_nums(sockfd, {&type, (rw_t*) &a});
            rw_t id = get_res(sockfd, this);
            long_queries[id] = SQRT;
            pthread_mutex_unlock(&mutex);
            cout << "Query " << command << " is queued with id " << id << endl;
        } else if (type == 5) {
            fact_t n;
            cin >> n;
            pthread_mutex_lock(&mutex);
            write_nums(sockfd, {&type, (rw_t*) &n});
            rw_t id = get_res(sockfd, this);
            long_queries[id] = FACT;
            pthread_mutex_unlock(&mutex);
            cout << "Query " << command << " is queued with id " << id << endl;
        }
    }
}

pthread_mutex_t *Client::get_mutex() { return &mutex; }
type Client::get_long_query(rw_t id) { return long_queries[id];}

task_args::task_args(Client *client, int sockfd): client(client), sockfd(sockfd) {}
rw_exception::rw_exception(const string &what): runtime_error(what) {}