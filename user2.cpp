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
        std::cerr << "Failed to initialize connection." << std::endl; // Error initializing connection
        return 1;
    }

    std::cout << "Connection initialized" << std::endl;

    // Sending an object of type MyClass
    MyClass obj(3, "name of the object");
    comm.sendMessages(socket, &obj, sizeof(obj));

    // Wait for a short period to ensure the message is sent
    std::this_thread::sleep_for(std::chrono::seconds(1));

    char str2[] = "bbbbb";
    comm.sendMessages(socket, (void*)str2, strlen(str2));

    // Keep the program running to maintain the connection and receive messages
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10)); // Sleep to keep the process alive
    }
    return 0;
}
