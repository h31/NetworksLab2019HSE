#include <cstdio>
#include <iostream>

#import "client.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Invalid arguments";
        exit(0);
    }

    uint16_t port_number = static_cast<uint16_t>(std::stoi(argv[2]));
    FileSystemClient fileSystemClient = FileSystemClient(argv[1], 5005);
    fileSystemClient.downloadFile("hello");
    std::string line;
    char dest[256], src[256];
    while (true) {
        getline(std::cin, line);
        if (sscanf(line.c_str(), "upload %s %s", dest, src)) {
            fileSystemClient.uploadFile(dest, src);
        } else if (sscanf(line.c_str(), "download %s", dest)) {
            fileSystemClient.downloadFile(dest);
        } else if (line == "ls") {
            fileSystemClient.getFilesList();
        } else if (sscanf(line.c_str(), "cd %s", dest)) {
            fileSystemClient.moveToUrl(dest);
        } else if (line == "exit") {
            fileSystemClient.quit();
            break;
        } else {
            std::cout << "Unknown command: " << line << '\n';
        }
    }

    return 0;
}