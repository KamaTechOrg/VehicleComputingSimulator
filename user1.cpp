#include "communication.h"
#include "myClass.cpp"
#include <thread>
#include <cstring>
#include <iostream>

int main() {
    communication comm;
<<<<<<< HEAD
    
    // Initialize the connection and get the socket descriptor
    int socket = comm.initConnection();
=======
    int portNumber = 8080;
    int socket = comm.initConnection(portNumber);
>>>>>>> 4f56224 (basic api with testing)
    if (socket < 0) {
        std::cerr << "Failed to initialize connection." << std::endl;
        return 1;
    }

    std::cout << "Connection initialized" << std::endl;

<<<<<<< HEAD
    const char* str = "cccc";
    comm.sendMessages(socket, (void*)str, strlen(str));
    // Wait for a short period to ensure the message is sent
    std::this_thread::sleep_for(std::chrono::seconds(1));

    char str2[] = "dddd";
    comm.sendMessages(socket, (void*)str2, strlen(str2));

    // Keep the program running to maintain the connection and receive messages
=======
    // שליחת אובייקט מסוג MyClass

    const char* str = "aaaaaaaaaaaaaaaaa";
    comm.sendMessages(socket, (void*)str, sizeof(str));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    char str2[] = "sssssssss";
    comm.sendMessages(socket, (void*)str2, sizeof(str));

    // שמירה על ההאזנה פעילה
>>>>>>> 4f56224 (basic api with testing)
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    return 0;
<<<<<<< HEAD
}
=======
}
>>>>>>> 4f56224 (basic api with testing)
