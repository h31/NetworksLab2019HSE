#include "Server.h"

void Server::init_network(uint16_t port)
{
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (_sockfd < 0) {
        throw NetworkException("Could not open socket");
    }

    struct sockaddr_in serv_addr;

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        throw NetworkException("Could not bind");
    }
}

Server::Server(uint16_t port)
{
    init_network(port);
}

int Server::accept_client()
{
    int sockfd;
    unsigned int clilen;
    struct sockaddr_in cli_addr;

    listen(_sockfd, 5);
    clilen = sizeof(cli_addr);

    sockfd = accept(_sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if (sockfd < 0) {
        throw NetworkException("Could not accept");
    }

    return sockfd;
}

void Server::send_message(int sockfd, char* x, size_t size)
{
    if (write(sockfd, x, size) < 0) {
        throw NetworkException("Could not write message");
    }

    std::cout << "send:     float: " << (*((float_t*)x)) << "     int: " << (*((uint_t*)x)) << std::endl;
}

void Server::read_message(int sockfd, char* x, size_t size)
{
    if (read(sockfd, x, size) < 0) {
        throw NetworkException("Could not read message");
    }

    std::cout << "read:     float: " << (*((float_t*)x)) << "     int: " << (*((uint_t*)x)) << std::endl;
}

void* long_operation(void *void_params)
{
    TaskData *task_data = (TaskData*)void_params;
    int sockfd = task_data->sockfd;

    Server::code_t command_type = task_data->command_type;
    
    try {
        std::cout << "------ id: " << (task_data->id) << std::endl;
        std::cout << "------ command_type: " << (command_type) << std::endl;

        pthread_mutex_lock(task_data->mutex_sockfd);
        std::cout << "------ a" << std::endl;
        Server::uint_t code = Server::CODE_LONG_ID;
        std::cout << "------ b" << std::endl;
        std::cout << "------ code: " << code << std::endl;
        Server::send_message(sockfd, (char*)&code, sizeof(code));
        std::cout << "------ c" << std::endl;
        Server::send_message(sockfd, (char*)(&(task_data->id)), sizeof(task_data->id));
        std::cout << "------ d" << std::endl;
        pthread_mutex_unlock(task_data->mutex_sockfd);
        std::cout << "------ e" << std::endl;

        Server::uint_t tmp_uint;
        Server::float_t tmp_float;
        char* res_buff = NULL;
        size_t res_size;
        std::cout << "------ f" << std::endl;
        switch(command_type) {
            case Server::CODE_GET_FACT:
                tmp_uint = *((Server::uint_t*)(task_data->param));
                tmp_uint = Calculator::get_fact(tmp_uint);
                res_buff = (char*)&tmp_uint;
                res_size = sizeof(tmp_uint);
                break;

            case Server::CODE_GET_SQRT:
                tmp_float = *((Server::float_t*)(task_data->param));
                std::cout << "------ tmp_float: " << tmp_float << std::endl;
                tmp_float = Calculator::get_sqrt(tmp_float);
                std::cout << "------ tmp_float res: " << tmp_float << std::endl;
                res_buff = (char*)&tmp_float;
                res_size = sizeof(tmp_float);
                break;
        }

        std::cout << "------ g" << std::endl;
        pthread_mutex_lock(task_data->mutex_sockfd);
        code = Server::CODE_LONG;
        std::cout << "------ h" << std::endl;
        Server::send_message(sockfd, (char*)&code, sizeof(code));
        std::cout << "------ i" << std::endl;
        Server::send_message(sockfd, (char*)(&(task_data->id)), sizeof(task_data->id));
        std::cout << "------ j" << std::endl;
        Server::send_message(sockfd, res_buff, res_size);
        std::cout << "------ k" << std::endl;
        pthread_mutex_unlock(task_data->mutex_sockfd);
        std::cout << "------ l" << std::endl;
    } catch (NetworkException &e) {
        std::cout << "------ fuuuuuuuuuuuck" << std::endl;
        pthread_mutex_unlock(task_data->mutex_sockfd);
    }
    
    task_data->mark_finished();
}

void* client_service(void *void_params)
{
    std::cout << "started thread" << std::endl;
    TaskData *task_data = (TaskData*)void_params;
    int sockfd = task_data->sockfd;

    TaskController task_controller;
    Server::uint_t id = 1;

    std::cout << 0 << std::endl;
    //while (true) {
    for (int itr = 0; itr < 2; itr++) {
        try {
            Server::code_t command_type;
            std::cout << 1 << std::endl;
            Server::read_message(sockfd, (char*)&command_type, sizeof(command_type));
            std::cout << 2 << std::endl;

            if (command_type == Server::CODE_GET_FACT || command_type == Server::CODE_GET_SQRT) {
                char* buff;
                if (command_type == Server::CODE_GET_FACT) {
                    buff = new char[sizeof(Server::uint_t)];
                    Server::read_message(sockfd, buff, sizeof(Server::uint_t));
                } else {
                    buff = new char[sizeof(Server::float_t)];
                    Server::read_message(sockfd, buff, sizeof(Server::float_t));
                }

                pthread_t *thread = (pthread_t*)malloc(sizeof(pthread_t));
                TaskData *long_task_data = new TaskData(*task_data, thread, command_type, id++, buff);
                pthread_create(thread, NULL, long_operation, (void*)long_task_data);

                task_controller.add(long_task_data);
            } else {
                std::cout << 3 << std::endl;
                Server::float_t a, b, c;
                Server::read_message(sockfd, (char*)&a, sizeof(a));
                Server::read_message(sockfd, (char*)&b, sizeof(b));
                std::cout << 4 << std::endl;

                switch(command_type) {
                    case Server::CODE_GET_SUM:
                        c = Calculator::get_sum(a, b);
                        break;

                    case Server::CODE_GET_DIFF:
                        c = Calculator::get_diff(a, b);
                        break;

                    case Server::CODE_GET_MUL:
                        c = Calculator::get_mul(a, b);
                        break;

                    case Server::CODE_GET_QUOT:
                        c = Calculator::get_quot(a, b);
                        break;

                    default:
                        throw NetworkException("Unknown command");
                        break;
                }
                std::cout << 5 << std::endl;

                try {
                    pthread_mutex_lock(task_data->mutex_sockfd);
                    Server::uint_t code = Server::CODE_FAST;
                    Server::send_message(sockfd, (char*)&code, sizeof(code));
                    Server::send_message(sockfd, (char*)&c, sizeof(c));
                    pthread_mutex_unlock(task_data->mutex_sockfd);
                    std::cout << 6 << std::endl;
                } catch (NetworkException &e) {
                    pthread_mutex_unlock(task_data->mutex_sockfd);
                    throw e;
                }
                std::cout << 8 << std::endl;
            }
        } catch (NetworkException &e) {
            std::cout << "break" << std::endl;
            break;
        }
        std::cout << 9 << std::endl;

        task_controller.tryFinish();

        std::cout << 10 << std::endl;
    }
    std::cout << 11 << std::endl;

    task_controller.finish();
    task_data->mark_finished();
}

void Server::run()
{
    TaskController task_controller;

    while (true) {
        int sockfd;
        try {
            sockfd = accept_client();
        } catch (NetworkException &e) {
            continue;
        }

        pthread_t *thread = (pthread_t*)malloc(sizeof(pthread_t));
        TaskData *task_data = new TaskData(thread, sockfd);
        pthread_create(thread, NULL, client_service, (void*)task_data);

        task_controller.add(task_data);
        task_controller.tryFinish();
    }

    task_controller.finish();
}

Server::~Server()
{
    close(_sockfd);
}