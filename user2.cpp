#include "communication.h"
#include "myClass.cpp"
#include <thread>
#include <cstring>
#include <iostream>

int main() {
    communication comm;
    // Set the callback for data received
    comm.setDataReceivedCallback([](const std::vector<uint8_t>& data) {
        std::cout << "Data received callback: ";
        for (auto byte : data) {
            std::cout << static_cast<char>(byte); // Print as characters
        }
        std::cout << std::endl;
    });

    // Set the callback for acknowledgment
    comm.setAckCallback([](AckType ack) {
        std::cout << "Acknowledgment received: " << (ack == AckType::ACK ? "ACK" : "NACK") << std::endl;
    });

    // Initialize the connection and get the socket descriptor
    int socket = comm.initConnection();
    if (socket < 0) {
        std::cerr << "Failed to initialize connection." << std::endl; // Error initializing connection
        return 1;
    }

    MyClass obj(3, "name of the object");

    comm.sendMessages(socket, &obj, sizeof(obj));
    // Wait for a short period to ensure the message is sent
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Example message to send
    char str2[] = "bbbbb";
    comm.sendMessages(socket, (void*)str2, strlen(str2));

    // Keep the program running to maintain the connection and receive messages
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10)); // Sleep to keep the process alive
    }
    return 0;
}
