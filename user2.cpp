#include "communication.h"
#include "myClass.cpp"
#include <thread>
#include <cstring>
#include <iostream>

int main() {
    communication comm;
    int portNumber = 8081;
    int socket = comm.initConnection(portNumber);
    if (socket < 0) {
        std::cerr << "Failed to initialize connection." << std::endl;
        return 1;
    }

    std::cout << "Connection initialized" << std::endl;

    // שליחת אובייקט מסוג MyClass
    MyClass obj2(2, "Object from user2");
    comm.sendMessages(socket, &obj2, sizeof(obj2));

    // שמירה על ההאזנה פעילה
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    return 0;
}