#include "communication.h"
#include "myClass.cpp"
#include <thread>
#include <cstring>
#include <iostream>

int main() {
    communication comm;
    int portNumber = 8081; // Port number for this connection
    int socket = comm.initConnection(portNumber);
    if (socket < 0) {
        std::cerr << "Failed to initialize connection." << std::endl; // Error initializing connection
        return 1;
    }

    std::cout << "Connection initialized" << std::endl;

    // Sending an object of type MyClass
    MyClass obj2(2, "Object from user2");
    comm.sendMessages(socket, &obj2, sizeof(obj2));

    // Keeping the listening active
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10)); // Sleep to keep the process alive
    }

    return 0;
}
