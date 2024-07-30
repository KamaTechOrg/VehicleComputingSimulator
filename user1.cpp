#include "communication.h"
#include "myClass.cpp"
#include <thread>
#include <cstring>
#include <iostream>

int main() {
    communication comm;
    int portNumber = 8080;
    int socket = comm.initConnection(portNumber);
    if (socket < 0) {
        std::cerr << "Failed to initialize connection." << std::endl;
        return 1;
    }

    std::cout << "Connection initialized" << std::endl;

    // שליחת אובייקט מסוג MyClass

    const char* str = "aaaaaaaaaaaaaaaaa";
    comm.sendMessages(socket, (void*)str, sizeof(str));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    char str2[] = "sssssssss";
    comm.sendMessages(socket, (void*)str2, sizeof(str));

    // שמירה על ההאזנה פעילה
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    return 0;
}