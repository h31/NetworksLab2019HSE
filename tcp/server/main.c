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
    GHashTable *devs;
    GHashTable *bugs;
    GHashTable *closed_bugs;
    pthread_mutex_t client_mutex;
    pthread_mutex_t logins_mutex;
    pthread_mutex_t devs_mutex;
} Server_t;

typedef struct Client_arg {
    int newsockfd;
    Server_t *server;
} Client_arg_t;

typedef struct Client {
    char *name;
    pthread_t t;
} Client_t;

//typedef struct Project {
//    char *id;
//    GHashTable
//} Project_t;

typedef struct Dev {
    char *id;
    GHashTable *projects;
} Dev_t;

typedef struct Bug {
    char *id;
    int closed;
    int fixed;
    char *proj_id;
    Dev_t *dev;
    char *text;
} Bug_t;

void dev_session(Client_arg_t *pArg);

void qa_session(Client_arg_t *cl_arg);

void process_list_closed_bugs(Client_arg_t *pArg);

void process_list_active_bugs(Client_arg_t *pArg);

void process_report_new_bug(Client_arg_t *cl_arg, const char *buffer);

void process_review_bug_fix(Client_arg_t *pArg);

void printf_tid(char *msg, char *buffer) {
    printf("tid %llu: %s: %s\n", (unsigned long long) pthread_self(), msg, buffer);
}

void clients_iterator(gpointer key, gpointer value, gpointer user_data) {
    //мб нужно kill
    int a = pthread_kill(*(pthread_t *) key, NULL);
}

void stop_server(Server_t *server) {
    pthread_mutex_lock(&server->client_mutex);
    g_hash_table_foreach(server->clients, (GHFunc) clients_iterator, NULL);
    g_hash_table_destroy(server->clients);
    pthread_mutex_unlock(&server->client_mutex);
}

void delete_client(Server_t *server) {
    //todo
    pthread_mutex_lock(&server->client_mutex);
//    g_set_
//    server->clients;
    pthread_mutex_unlock(&server->client_mutex);
}

gboolean get_val(gpointer key, gpointer value, gpointer user_data) {
    gchar *pkey = (gchar *) key;
    gchar *pdata = (gchar *) user_data;
    printf("get_val: %s and %s \n", pkey, pdata);
    return !strcmp(pkey, pdata);
}

//0,1 - зарегались успешно
//-1 - ошибка
int reg_or_login(Client_arg_t *cl_arg) {
    ssize_t n;
    char buffer[256];
    bzero(buffer, 256);
    while (1) {
        n = read(cl_arg->newsockfd, buffer, 255); // recv on Windows
        if (n < 0) {
            srverror("reg_or_login: ERROR reading from socket");
            return -1;
        } else if (n == 0) {
            printf("Client's socket closed\n");
            return -1;
        }
        printf_tid("Client's registration msg:", buffer);
        if (!strncmp(buffer, "REGT ", 5)) {
            int i = 5;
            char login[256];
            for (; i < strlen(buffer) && buffer[i] != '\n'; ++i) {
                login[i - 5] = buffer[i];
            }
            pthread_mutex_lock(&cl_arg->server->logins_mutex);
            gpointer p = g_hash_table_find(cl_arg->server->logins, (GHRFunc) get_val, login);
            pthread_mutex_unlock(&cl_arg->server->logins_mutex);
            if (p == NULL) {
                printf("REGT: free login\n");
                pthread_mutex_lock(&cl_arg->server->logins_mutex);
                printf("Inserting data: %s", login);
                g_hash_table_insert(cl_arg->server->logins, g_strdup(login), "T");
                pthread_mutex_unlock(&cl_arg->server->logins_mutex);
                bzero(buffer, 256);
                strcpy(buffer, "REGT OK \n");
                printf("Send: %s\n", buffer);
                n = write(cl_arg->newsockfd, buffer, strlen(buffer));
                if (n < 0) {
                    srverror("ERROR writing to socket");
                    return -1;
                } else if (n == 0) {
                    printf("Client's socket closed\n");
                    return -1;
                }
                return 1;
            } else {
                printf("REGT: login already in use\n");
                bzero(buffer, 256);
                strcpy(buffer, "REGT UE \n");
                printf("Send: %s\n", buffer);
                n = write(cl_arg->newsockfd, buffer, strlen(buffer));
                if (n < 0) {
                    srverror("ERROR writing to socket");
                    return -1;
                } else if (n == 0) {
                    printf("Client's socket closed\n");
                    return -1;
                }
            }
        } else if (!strncmp(buffer, "REGD ", 5)) {
            int i = 5;
            char login[256];
            for (; i < strlen(buffer) && buffer[i] != '\n'; ++i) {
                login[i - 5] = buffer[i];
            }
            pthread_mutex_lock(&cl_arg->server->logins_mutex);
            gpointer p = g_hash_table_find(cl_arg->server->logins, (GHRFunc) get_val, login);
            pthread_mutex_unlock(&cl_arg->server->logins_mutex);
            if (p == NULL) {
                printf("REGD: free login\n");
                pthread_mutex_lock(&cl_arg->server->logins_mutex);
                printf("Inserting data: %s", login);
                g_hash_table_insert(cl_arg->server->logins, g_strdup(login), "D");
                pthread_mutex_unlock(&cl_arg->server->logins_mutex);
                //create new dev
                pthread_mutex_lock(&cl_arg->server->devs_mutex);
                //todo delete для Dev
                Dev_t *dev = malloc(sizeof(*dev));
                dev->id = login;
                dev->projects = g_hash_table_new(g_str_hash, g_str_equal);
                //test proj
                g_hash_table_insert(dev->projects, "1", "");
                pthread_mutex_unlock(&cl_arg->server->devs_mutex);
                bzero(buffer, 256);
                strcpy(buffer, "REGD OK \n");
                printf("Send: %s\n", buffer);
                n = write(cl_arg->newsockfd, buffer, strlen(buffer));
                if (n < 0) {
                    srverror("ERROR writing to socket");
                    return -1;
                } else if (n == 0) {
                    printf("Client's socket closed\n");
                    return -1;
                }
                return 0;
            } else {
                printf("REGD: login already in use\n");
                bzero(buffer, 256);
                strcpy(buffer, "REGD UE \n");
                printf("Send: %s\n", buffer);
                n = write(cl_arg->newsockfd, buffer, strlen(buffer));
                if (n < 0) {
                    srverror("ERROR writing to socket");
                    return -1;
                } else if (n == 0) {
                    printf("Client's socket closed\n");
                    return -1;
                }
            }
        } else if (!strncmp(buffer, "AUTH ", 5)) {
            int i = 5;
            char login[256];
            for (; i < strlen(buffer) && buffer[i] != '\n'; ++i) {
                login[i - 5] = buffer[i];
            }
            pthread_mutex_lock(&cl_arg->server->logins_mutex);
            gpointer p = g_hash_table_find(cl_arg->server->logins, (GHRFunc) get_val, login);
            pthread_mutex_unlock(&cl_arg->server->logins_mutex);
            if (p == NULL) {
                printf("AUTH: no such login\n");
                bzero(buffer, 256);
                strcpy(buffer, "AUTH UN \n");
                printf("Send: %s\n", buffer);
                n = write(cl_arg->newsockfd, buffer, strlen(buffer));
                if (n < 0) {
                    srverror("ERROR writing to socket");
                    return -1;
                } else if (n == 0) {
                    printf("Client's socket closed\n");
                    return -1;
                }
            } else {
                printf("AUTH: hashtable item:");
                printf(p);
                printf("\n");
                printf("AUTH: found login\n");
                bzero(buffer, 256);
                int kind;
                if (!strcmp((char *) p, "T")) {
                    strcpy(buffer, "AUTH OK T\n");
                    kind = 1;
                } else {
                    strcpy(buffer, "AUTH OK D\n");
                    kind = 0;
                }
                printf("Send: %s\n", buffer);
                n = write(cl_arg->newsockfd, buffer, strlen(buffer));
                if (n < 0) {
                    srverror("ERROR writing to socket");
                    return -1;
                } else if (n == 0) {
                    printf("Client's socket closed\n");
                    return -1;
                }
                return kind;
            }
        } else {
            srverror("Bad registration\n");
            close(cl_arg->newsockfd);
            //todo
            delete_client(cl_arg);
            bzero(buffer, 256);
            strcpy(buffer, "BR \n");
            n = write(cl_arg->newsockfd, buffer, strlen(buffer));
            return -1;
        }
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

    int kind = reg_or_login(cl_arg);

    if (kind == -1) return 0;

    if (kind == 0) {
        dev_session(cl_arg);
    } else if (kind == 1) {
        qa_session(cl_arg);
    } else {
        srverror("reg_or_login returned something different from 0/-1/1");
    }

    return 0;
}

void qa_session(Client_arg_t *cl_arg) {
    ssize_t n;
    char buffer[256];
    bzero(buffer, 256);
    while (1) {
        n = read(cl_arg->newsockfd, buffer, 255);
        if (n < 0) {
            srverror("qa_session: ERROR reading from socket");
            return;
        } else if (n == 0) {
            printf("qa_session: Client's socket closed\n");
            return;
        }
        printf_tid("qa's msg:", buffer);
        if (!strncmp(buffer, "BCLS ", 5)) {
            int i = 5;
            char login[256];
            for (; i < strlen(buffer) && buffer[i] != '\n'; ++i) {
                login[i - 5] = buffer[i];
            }
            process_list_closed_bugs(cl_arg);
        } else if (!strncmp(buffer, "BACT ", 5)) {
            process_list_active_bugs(cl_arg);
        } else if (!strncmp(buffer, "BREP ", 5)) {
            process_report_new_bug(cl_arg, buffer);
        } else if (!strncmp(buffer, "BREV ", 5)) {
            process_review_bug_fix(cl_arg);
        } else {
            srverror("dev_session: Bad request\n");
            close(cl_arg->newsockfd);
            //todo нормально освободить ресурсы
            delete_client(cl_arg);
            bzero(buffer, 256);
            strcpy(buffer, "BR \n");
            n = write(cl_arg->newsockfd, buffer, strlen(buffer));
            return;
        }
    }
}

void process_review_bug_fix(Client_arg_t *pArg) {

}

void write_br(Client_arg_t *cl_arg) {
    ssize_t n;
    char answer[10];
    strcat(answer, "BR \n");
    n = write(cl_arg->newsockfd, answer, strlen(answer));
    if (n < 0) {
        srverror("ERROR writing to socket");
        return;
    } else if (n == 0) {
        printf("Client's socket closed\n");
        return;
    }
}

void process_report_new_bug(Client_arg_t *cl_arg, const char *buffer) {
    ssize_t n;
//    bzero(buffer, 256);
    /*n = read(cl_arg->newsockfd, buffer, 255);
    if (n < 0) {
        srverror("process_report_new_bug: ERROR reading from socket");
        return;
    } else if (n == 0) {
        printf("process_report_new_bug: Client's socket closed\n");
        return;
    }*/
    int i = 5;
    char dev_id[256] = "\0";
    char proj_id[256] = "\0";
    char bug_id[256] = "\0";
    char bug_text[256] = "\0";
    while (buffer[i] != '|' && i < strlen(buffer)) {
        char curChar[2] = "\0";
        curChar[0] = buffer[i];
        strcat(dev_id, curChar);
        i++;
    }
    if (buffer[i] != '|' || i >= strlen(buffer) - 2) {
        write_br(cl_arg);
        return;
    }
    i++;
    while (buffer[i] != '|' && i < strlen(buffer)) {
        char curChar[2] = "\0";
        curChar[0] = buffer[i];
        strcat(proj_id, curChar);
        i++;
    }
    if (buffer[i] != '|' || i >= strlen(buffer) - 2) {
        write_br(cl_arg);
        return;
    }
    i++;
    while (buffer[i] != '|' && i < strlen(buffer)) {
        char curChar[2] = "\0";
        curChar[0] = buffer[i];
        strcat(bug_id, curChar);
        i++;
    }
    if (buffer[i] != '|' || i >= strlen(buffer) - 2) {
        write_br(cl_arg);
        return;
    }
    i++;
    while (buffer[i] != '|' && i < strlen(buffer)) {
        char curChar[2] = "\0";
        curChar[0] = buffer[i];
        strcat(bug_text, curChar);
        i++;
    }
    if (i < strlen(buffer) && buffer[i] != '|') {
        write_br(cl_arg);
        return;
    }
    printf("Dev_id: %s\n", dev_id);
    printf("Proj_id: %s\n", proj_id);
    printf("Bug_id: %s\n", bug_id);
    printf("Bug_text: %s\n", bug_text);
}

void process_list_active_bugs(Client_arg_t *pArg) {

}

void process_list_closed_bugs(Client_arg_t *pArg) {

}

void dev_session(Client_arg_t *pArg) {
    ssize_t n;
    char buffer[256];
    bzero(buffer, 256);
    while (1) {

    }
}

void create_client(Server_t *server, Client_arg_t *client_arg) {
    pthread_t pthread;
    pthread_create(&pthread, NULL, client_worker, client_arg);
    pthread_mutex_lock(&server->client_mutex);
    g_hash_table_add(server->clients, &pthread);
    pthread_mutex_unlock(&server->client_mutex);
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
    server.devs = g_hash_table_new(g_str_hash, g_str_equal);
    pthread_mutex_init(&server.client_mutex, NULL);
    pthread_mutex_init(&server.logins_mutex, NULL);
    pthread_mutex_init(&server.devs_mutex, NULL);

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