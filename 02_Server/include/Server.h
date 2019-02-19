#pragma once

#include <stddef.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <pthread.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstdint>
#include <vector>

#include <stddef.h>
#include <cstdint>
#include "Calculator.h"

struct NetworkException
{
private:
    std::string message;

public:
    NetworkException(std::string message)
    : message(message)
    {
    }

    std::string get_message() const
    {
        return message;
    }
};

class Server
{
private:
    int _sockfd;

    void init_network(uint16_t port);
    int accept_client();

public:
	typedef uint64_t code_t;
    typedef uint64_t uint_t;
    typedef double float_t;

    static const code_t CODE_FAST = 1;
    static const code_t CODE_LONG_ID = 2;
    static const code_t CODE_LONG = 3;

    static const code_t CODE_GET_SUM = 1;
    static const code_t CODE_GET_DIFF = 2;
    static const code_t CODE_GET_MUL = 3;
    static const code_t CODE_GET_QUOT = 4;
    static const code_t CODE_GET_FACT = 5;
    static const code_t CODE_GET_SQRT = 6;

    Server(uint16_t port);
    ~Server();

    static void read_message(int sockfd, char* x, size_t size);
    static void send_message(int sockfd, char* x, size_t size);

    void run();
};

struct TaskData
{
private:
    bool is_long_operation;
    bool finished;

    void init_state()
    {
        finished = false;
        mutex_is_finished = new pthread_mutex_t;
        pthread_mutex_init(mutex_is_finished, NULL);
    }

public:
    pthread_t *thread;
    int sockfd;

    Server::code_t command_type;
    Server::uint_t id;
    char* param = NULL;

    pthread_mutex_t *mutex_sockfd;
    pthread_mutex_t *mutex_is_finished;

    TaskData(pthread_t *thread, int sockfd)
    : thread(thread), sockfd(sockfd)
    {
        is_long_operation = false;

        init_state();

        mutex_sockfd = new pthread_mutex_t;
        pthread_mutex_init(mutex_sockfd, NULL);
    }

    TaskData(TaskData &other, pthread_t *thread, Server::code_t command_type, Server::uint_t id, char* param)
    : thread(thread), command_type(command_type), id(id), param(param)
    {
        is_long_operation = true;

        init_state();

        sockfd = other.sockfd;
        mutex_sockfd = other.mutex_sockfd;
    }

    ~TaskData()
    {
        pthread_join(*thread, NULL);

        if (param != NULL) {
            delete[] param;
        }
        free(thread);

        pthread_mutex_destroy(mutex_is_finished);

        if (!is_long_operation) {
            close(sockfd);
            pthread_mutex_destroy(mutex_sockfd);
        }
    }

    void mark_finished()
    {
        pthread_mutex_lock(mutex_is_finished);
        finished = true;
        pthread_mutex_unlock(mutex_is_finished);
    }

    bool is_finished()
    {
        pthread_mutex_lock(mutex_is_finished);
        bool ans = finished;
        pthread_mutex_unlock(mutex_is_finished);
        return ans;
    }
};

class TaskController
{
private:
    std::vector<TaskData*> tasks;

public:
    TaskController() 
    {}

    void add(TaskData *task)
    {
        tasks.push_back(task);
    }

    void tryFinish()
    {
        std::vector<TaskData*> remaining_tasks;
        for (TaskData *task : tasks)
        {
            if (task->is_finished())
            {
                delete task;
            } else {
                remaining_tasks.push_back(task);
            }
        }

        tasks = remaining_tasks;
    }

    void finish()
    {
        while (tasks.size() > 0) {
            tryFinish();
        }
    }
};