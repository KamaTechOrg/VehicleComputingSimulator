#include "communication.h"
#include "myClass.cpp"
#include <thread>
#include <cstring>
#include <iostream>

int main() {
    communication comm;
    
    // Initialize the connection and get the socket descriptor
    int socket = comm.initConnection();
    if (socket < 0) {
        std::cerr << "Failed to initialize connection." << std::endl;
        return 1;
    }

    std::cout << "Connection initialized" << std::endl;

<<<<<<< HEAD
    // Sending an object of type MyClass

    const char* str = "bbbbbbb";
    comm.sendMessages(socket, (void*)str, sizeof(str));
||||||| parent of 9052823 (Clean the code, add comments)
    // שליחת אובייקט מסוג MyClass

    const char* str = "bbbbbbb";
    comm.sendMessages(socket, (void*)str, sizeof(str));
=======
    const char* str = "cccc";
    comm.sendMessages(socket, (void*)str, strlen(str));
    // Wait for a short period to ensure the message is sent
>>>>>>> 9052823 (Clean the code, add comments)
    std::this_thread::sleep_for(std::chrono::seconds(1));

    char str2[] = "dddd";
    comm.sendMessages(socket, (void*)str2, strlen(str2));

<<<<<<< HEAD
    // Keeping the listening active
||||||| parent of 9052823 (Clean the code, add comments)
    // שמירה על ההאזנה פעילה
=======
    // Keep the program running to maintain the connection and receive messages
>>>>>>> 9052823 (Clean the code, add comments)
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    return 0;
}
