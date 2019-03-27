#include <utility>

#include "Task.h"

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <regex>
#include <fstream>
#include <iostream>

Task::Task(int socket, std::string root_directory) {
    this->socket = socket;
    this->directory = root_directory;
    this->root_directory = root_directory;
}

void Task::cd_in_dir() {
    std::regex short_path("/[^/]*\\/\\.\\.");
    std::string path = get_string();
    std::string new_path = std::regex_replace(this->directory + path, short_path, "");
    if (new_path[new_path.size() - 1] != '/') {
        new_path.push_back('/');
    }
    if (new_path.length() >= root_directory.length() &&
        strncmp(new_path.c_str(), root_directory.c_str(), root_directory.length()) == 0
            && opendir(new_path.c_str())) {
        directory = new_path;
        send_num(CD_IN_DIR_SUCC);
        if (new_path == root_directory) {
            send_string("/");
        }
        send_string(directory.substr(root_directory.size(), directory.size() - root_directory.size()));
    } else {
        send_num(CD_IN_DIR_FAIL);
        send_string(directory.substr(root_directory.size(), directory.size() - root_directory.size()) + path);
    }
}

void Task::get_file_list() {
    get_num();
    std::vector<std::string> file_list = get_file_list_in_dir();
    int message_length = sizeof(int);
    for (auto &i : file_list) {
        message_length += i.length() + sizeof(int);
    }
    int file_number = file_list.size();
    int answer = GET_FILE_LIST_SUCC;
    send_num(answer);
    send_num(message_length);
    send_num(file_number);
    for (auto &i : file_list) {
        send_string(i);
    }
}

void Task::get_file() {
    get_num();
    std::string file_name = get_string();
    FILE *fp;
    if((fp = fopen(file_name.c_str(), "w+b")) == NULL) {
        int answer = GET_FILE_FAIL;
        send_num(answer);
        send_string(file_name);
        return;
    }
    int size = get_num();
    char* buf = new char[size];
    int n = read(socket, buf, size);
    int already_read = n;
    while (n > 0 && already_read < size) {
        n = read(socket, buf + already_read, size - already_read);
        already_read += n;
    }
    fwrite (buf , sizeof(char), size, fp);
    delete[] buf;
    fclose(fp);
    if (n < 0) {
        terminate();
    }
    int answer = GET_FILE_SUCC;
    send_num(answer);
    send_string(file_name.c_str());
}

void Task::send_file() {
    std::string file_name = get_string();
    FILE *fp;
    if((fp = fopen(file_name.c_str(), "rb")) == NULL) {
        send_num(SEND_FILE_FAIL);
        send_string(file_name);
        return;
    }
    fseek(fp , 0 , SEEK_END);
    int file_size = ftell(fp);
    rewind(fp);
    char* buf = new char[file_size];
    fread (buf, sizeof(char), file_size, fp);
    send_num(SEND_FILE_SUCC);
    send_num(file_size);
    write(socket, buf, file_size);
    delete[] buf;
}

void Task::send_string(std::string string_to_send) {
    int length = string_to_send.length();
    send_num(length);
    write(socket, string_to_send.c_str(), length);
}

std::string Task::get_string() {
    int length = get_num();
    char* buf = new char[length + 1];
    int n = read(socket, buf, length);
    int already_read = n;
    while (n > 0 && already_read < length) {
        n = read(socket, buf + already_read, length - already_read);
        already_read += n;
    }
    buf[length] = '\0';
    std::string ans(buf);
    delete[] buf;
    if (n < 0) {
        terminate();
    }
    return ans;
}

std::vector<std::string> Task::get_file_list_in_dir() {
    std::vector<std::string> result;
    struct dirent *entry;
    DIR *dir = opendir(directory.c_str());

    if (dir == NULL) {
        return result;
    }

    while ((entry = readdir(dir)) != NULL) {
        result.emplace_back(entry->d_name);
    }
    closedir(dir);
    return result;
}

int Task::get_num() {
    int num;
    int n = read(socket, &num, sizeof(int));
    if (n < 0) {
        terminate();
    }
    return ntohl(num);
}

void Task::send_num(int num) {
    num = htonl(num);
    write(socket, &num, sizeof(int));
}

void Task::terminate() {
    close(socket);
    socket = NULL;
    throw TaskException();
}

void clientWork(int socket, std::string root_dir) {
    Task task = Task(socket, std::move(root_dir));
    client_mutex.lock();
    clients.push_back(task);
    std::cout << "Create new client id " + clients.size();
    client_mutex.unlock();
    try {
        while (true) {
            int command = task.get_num();
            switch (command) {
                case CD_IN_DIR:
                    task.cd_in_dir();
                    break;
                case GET_FILE_LIST:
                    task.get_file_list();
                    break;
                case GET_FILE:
                    task.get_file();
                    break;
                case SEND_FILE:
                    task.send_file();
                    break;
                default:
                    task.send_num(UNKNOWN_REQUEST);
                    task.send_num(sizeof(int));
                    task.send_num(command);
                    break;
            }
        }
    } catch (Task::TaskException e) {
        return;
    }
}

void clientKiller() {
    while (true) {
        int client_id = -1;
        std::cin >> client_id;
        client_mutex.lock();
        if (client_id >= 0 && client_id < clients.size()) {
            clients[client_id].terminate();
        }
    }
}