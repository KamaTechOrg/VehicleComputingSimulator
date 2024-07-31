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
    int portNumber = 8081;
    int socket = comm.initConnection(portNumber);
>>>>>>> 4f56224 (basic api with testing)
    if (socket < 0) {
        std::cerr << "Failed to initialize connection." << std::endl;
        return 1;
    }

    std::cout << "Connection initialized" << std::endl;

<<<<<<< HEAD
    MyClass obj(3, "name of the object");

    comm.sendMessages(socket, &obj, sizeof(obj));
    // Wait for a short period to ensure the message is sent
    std::this_thread::sleep_for(std::chrono::seconds(1));

    char str2[] = "bbbbb";
    comm.sendMessages(socket, (void*)str2, strlen(str2));

    // Keep the program running to maintain the connection and receive messages
=======
    // שליחת אובייקט מסוג MyClass
    MyClass obj2(2, "Object from user2");
    comm.sendMessages(socket, &obj2, sizeof(obj2));

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
