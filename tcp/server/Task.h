#ifndef NETWORKS_TASK_H
#define NETWORKS_TASK_H
#include <string>
#include <dirent.h>
#include <sys/types.h>
#include <vector>
#include <mutex>

enum Tasks_types {
    CD_IN_DIR = 400,
    CD_IN_DIR_SUCC = 401,
    CD_IN_DIR_FAIL = 402,
    GET_FILE_LIST = 300,
    GET_FILE_LIST_SUCC = 301,
    GET_FILE = 100,
    GET_FILE_SUCC = 101,
    GET_FILE_FAIL = 102,
    SEND_FILE = 200,
    SEND_FILE_SUCC = 201,
    SEND_FILE_FAIL = 202,
    UNKNOWN_REQUEST = 1,
};

class Task {
    public:
        void cd_in_dir();
        void get_file_list();
        void get_file();
        void send_file();
        int get_num();
        void send_num(int num);
        void terminate();

        Task(int socket, std::string);

        class TaskException{};

    private:
        std::string directory;
        std::string root_directory;
        int socket;
        std::string get_string();
        void send_string(std::string string);
        std::vector<std::string> get_file_list_in_dir();
};

static std::vector<Task> clients;
static std::mutex client_mutex;

void clientWork(int socket, std::string root_dir);
void clientKiller();

#endif //NETWORKS_TASK_H
