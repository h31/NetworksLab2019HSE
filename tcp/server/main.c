#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>

#define srverror(msg) fprintf(stderr, "tid %llu: %s  Error: %s\n",\
                             (unsigned long long) pthread_self(), msg, strerror(errno))
#define srvprintf(msg) printf("tid %llu: %s", (unsigned long long) pthread_self(), msg)
#define TM_PRINTF(f_, ...) srvprintf((f_), ##__VA_ARGS__)
#define TM_PRINTF_TH(f_, ...) TM_PRINTF((printf(f_)), ##__VA_ARGS__)

typedef struct Server {
    int sockfd;
    GHashTable *clients;
    GHashTable *logins;
    pthread_mutex_t clients_array_mutex;
} Server_t;

typedef struct Client_arg {
    int newsockfd;
    Server_t *server;
} Client_arg_t;

typedef struct Client {
    char *name;
    pthread_t t;
} Client_t;

void clients_iterator(gpointer key, gpointer value, gpointer user_data) {
    //мб нужно kill
    int a = pthread_kill(*(pthread_t *) key, NULL);
}

void stop_server(Server_t *server) {
    pthread_mutex_lock(&server->clients_array_mutex);
    g_hash_table_foreach(server->clients, (GHFunc) clients_iterator, NULL);
    g_hash_table_destroy(server->clients);
    pthread_mutex_unlock(&server->clients_array_mutex);
}

void delete_client(Server_t *server) {
    //todo
    pthread_mutex_lock(&server->clients_array_mutex);
//    g_set_
//    server->clients;
    pthread_mutex_unlock(&server->clients_array_mutex);
}

gboolean get_val(gpointer key, gpointer value, gpointer user_data) {
    gchar *pkey = (gchar *) key;
    gchar *pdata = (gchar *) user_data;
    printf("get_val: %s and %s \n", pkey, pdata);
    return !strcmp(pkey, pdata);
}

int reg_or_login(Client_arg_t *cl_arg){
    ssize_t n;
    char buffer[256];
    bzero(buffer, 256);
    n = read(cl_arg->newsockfd, buffer, 255); // recv on Windows

    if (n < 0) {
        srverror("ERROR reading from socket");
        return -1;
    } else if (n == 0) {
        //todo
    }

    printf("tid %llu: %s: %s\n", (unsigned long long) pthread_self(), "Frst msg", buffer);
    if (!strncmp(buffer, "REGT ", 5)) {
        int i = 5;
        char login[256];
        for (; i < strlen(buffer) && buffer[i] != '\n'; ++i) {
            login[i - 5] = buffer[i];
        }
        pthread_mutex_lock(&cl_arg->server->clients_array_mutex);
        gpointer p = g_hash_table_find(cl_arg->server->logins, (GHRFunc) get_val, login);
        pthread_mutex_unlock(&cl_arg->server->clients_array_mutex);
        if (p == NULL) {
            printf("REGT: free login\n");
            pthread_mutex_lock(&cl_arg->server->clients_array_mutex);
            printf("Inserting data: %s", login);
            g_hash_table_insert(cl_arg->server->logins, g_strdup(login), "T");
            pthread_mutex_unlock(&cl_arg->server->clients_array_mutex);
            bzero(buffer, 256);
            strcpy(buffer, "REGT OK \n");
            printf("Send: %s\n", buffer);
            n = write(cl_arg->newsockfd, buffer, strlen(buffer));
            if (n < 0) {
                srverror("ERROR writing to socket");
                return 0;
            }
        } else {
            printf("REGT: login already in use\n");
            bzero(buffer, 256);
            strcpy(buffer, "REGT UE \n");
            printf("Send: %s\n", buffer);
            n = write(cl_arg->newsockfd, buffer, strlen(buffer));
            if (n < 0) {
                srverror("ERROR writing to socket");
                return 0;
            }
        }
    } else if (!strncmp(buffer, "REGD ", 5)) {

    } else {
        srverror("Bad registration\n");
        close(cl_arg->newsockfd);
        delete_client(cl_arg);
//        bzero(buffer, 256);
//        strcpy(buffer, "BR \n");
//        n = write(cl_arg->newsockfd, "", );
    }
}

void *client_worker(void *argp) {
    ssize_t n;
    Client_arg_t *cl_arg = (Client_arg_t *) argp;
    char buffer[256];

    if (cl_arg->newsockfd < 0) {
        srverror("ERROR on accept");
        return 0;
    }

    /* If connection is established then start communicating */
    bzero(buffer, 256);
    n = read(cl_arg->newsockfd, buffer, 255); // recv on Windows

    if (n < 0) {
        srverror("ERROR reading from socket");
        return 0;
    } else if (n == 0) {
        //todo
    }

    printf("tid %llu: %s: %s\n", (unsigned long long) pthread_self(), "Frst msg", buffer);
    if (!strncmp(buffer, "REGT ", 5)) {
        int i = 5;
        char login[256];
        for (; i < strlen(buffer) && buffer[i] != '\n'; ++i) {
            login[i - 5] = buffer[i];
        }
        pthread_mutex_lock(&cl_arg->server->clients_array_mutex);
        gpointer p = g_hash_table_find(cl_arg->server->logins, (GHRFunc) get_val, login);
        pthread_mutex_unlock(&cl_arg->server->clients_array_mutex);
        int tester = 0;
        if (p == NULL) {
            printf("REGT: free login\n");
            pthread_mutex_lock(&cl_arg->server->clients_array_mutex);
            printf("Inserting data: %s", login);
            g_hash_table_insert(cl_arg->server->logins, g_strdup(login), "T");
            pthread_mutex_unlock(&cl_arg->server->clients_array_mutex);
            bzero(buffer, 256);
            strcpy(buffer, "REGT OK \n");
            printf("Send: %s\n", buffer);
            n = write(cl_arg->newsockfd, buffer, strlen(buffer));
            if (n < 0) {
                srverror("ERROR writing to socket");
                return 0;
            }
            tester = 1;
        } else {
            printf("REGT: login already in use\n");
            bzero(buffer, 256);
            strcpy(buffer, "REGT UE \n");
            printf("Send: %s\n", buffer);
            n = write(cl_arg->newsockfd, buffer, strlen(buffer));
            if (n < 0) {
                srverror("ERROR writing to socket");
                return 0;
            }
        }
    } else if (!strncmp(buffer, "REGD ", 5)) {

    } else {
        srverror("Bad registration\n");
        close(cl_arg->newsockfd);
        delete_client(cl_arg);
//        bzero(buffer, 256);
//        strcpy(buffer, "BR \n");
//        n = write(cl_arg->newsockfd, "", );
    }

    /* Write a response to the client */
    n = write(cl_arg->newsockfd, "I got your message", 18); // send on Windows

    if (n < 0) {
        srverror("ERROR writing to socket");
        return 0;
    }
    return 0;
}

void create_client(Server_t *server, Client_arg_t *client_arg) {
    pthread_t pthread;
    pthread_create(&pthread, NULL, client_worker, client_arg);
    pthread_mutex_lock(&server->clients_array_mutex);
    g_hash_table_add(server->clients, &pthread);
    pthread_mutex_unlock(&server->clients_array_mutex);
}

void *server_listener(void *argp) {
    int newsockfd;
    Server_t *server = (Server_t *) argp;
    socklen_t clilen;
    struct sockaddr_in cli_addr;
    clilen = sizeof(cli_addr);

    while (1) {
        newsockfd = accept(server->sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            continue;
        }
        pthread_t pthread;
        const pthread_attr_t *attr;
        Client_arg_t cl_arg;
        bzero(&cl_arg, sizeof(cl_arg));
        cl_arg.server = server;
        cl_arg.newsockfd = newsockfd;
        create_client(server, &cl_arg);
    }
    stop_server(server);
    printf("Clients: %d", g_hash_table_size(server->clients));
    return 0;
}

//todo сделать main с kill/list
int main(int argc, char *argv[]) {
    Server_t server;
    int newsockfd;
    uint16_t portno;
    unsigned int clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    ssize_t n;

    bzero(&server, sizeof(server));
    /* First call to socket() function */
    server.sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server.clients = g_hash_table_new(g_int64_hash, g_int64_equal);
    server.logins = g_hash_table_new(g_str_hash, g_str_equal);
    pthread_mutex_init(&server.clients_array_mutex, NULL);

    if (server.sockfd < 0) {
        srverror("ERROR opening socket");
        exit(1);
    }

    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 5002;

    int enable = 1;
    if (setsockopt(server.sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        srverror("setsockopt(SO_REUSEADDR) failed");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    if (bind(server.sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        srverror("ERROR on binding");
        exit(1);
    }

    /* Now start listening for the clients, here process will
       * go in sleep mode and will wait for the incoming connection
    */

    listen(server.sockfd, 5);
    clilen = sizeof(cli_addr);

    pthread_t server_pthread;
    pthread_create(&server_pthread, NULL, server_listener, &server);
    while (1) {
        printf("len(clients)=%d | Available command: \"exit\", \"list\": ", g_hash_table_size(server.clients));

        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        if (!strcmp(buffer, "exit") || !strcmp(buffer, "exit\n")) {
            stop_server(&server);
            pthread_kill(server_pthread, 0);
            shutdown(server.sockfd, SHUT_RDWR);
            close(server.sockfd);
            exit(0);
        } else {
            printf("Unknown command\n");
        }
    }
}

int main1(int argc, char **argv) {
    return 0;
}