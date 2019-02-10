#include "server.h"

bool segment::operator < (segment other) {
    return left < other.left || (left == other.left && right < other.right);
}

Server::Server(int portno) {
    /* First call to socket() function */
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server_sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    struct sockaddr_in serv_addr, cli_addr;

     /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    if (bind(server_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    pthread_mutex_init(&mutex, NULL);
}

Server::~Server() {
    pthread_mutex_destroy(&mutex);
}

calc_t Server::get_max() {
    pthread_mutex_lock(&mutex);
    calc_t ans = max_val;
    pthread_mutex_unlock(&mutex);
    return ans;
}

vector<calc_t> Server::get_last_n(size_t n) {
    vector<calc_t> ans;
    pthread_mutex_lock(&mutex);
    for (int i = max(calculated.size(), n) - n; i < calculated.size(); i++) {
        ans.push_back(calculated[i]);    
    }
    pthread_mutex_unlock(&mutex);
    return ans;
}

void *doit(void *args) {
    char buffer[256];
    ssize_t n;
    int sockfd = ((task_args*) args)->sockfd;
    Server* server = ((task_args*) args)->server;

    /* If connection is established then start communicating */
    bzero(buffer, 256);

    printf("wait, it's read\n");
    fflush(stdout);
    n = read(sockfd, buffer, 255); // recv on Windows

    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    printf("Here is the message: %s\n", buffer);

    /* Write a response to the client */
    n = write(sockfd, "I got your message", 18); // send on Windows

    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
}

void Server::run() {
    while (true) {
        int sockfd;
        unsigned int clilen;
        struct sockaddr_in cli_addr;

        /* Now start listening for the clients, here process will
         * go in sleep mode and will wait for the incoming connection
        */
        listen(server_sockfd, 5);
        clilen = sizeof(cli_addr);


        printf("wait, it's accept\n");
        fflush(stdout);
        /* Accept actual connection from the client */
        sockfd = accept(server_sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (sockfd < 0) {
            perror("ERROR on accept");
            exit(1);
        }

        pthread_t *thread = (pthread_t*) malloc(sizeof(pthread_t));
        pthread_create(thread, NULL, doit, new task_args(this, sockfd));
    }
}

task_args::task_args(Server* server, int sockfd): server(server), sockfd(sockfd) {}