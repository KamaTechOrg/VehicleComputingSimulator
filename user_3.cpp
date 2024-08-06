#include <iostream>
#include "communication.hpp"

// Processing the data received from the complete message
void processData(void *data)
{
    std::cout << "Received data: " << static_cast<char *>(data) << std::endl;
    free(data); // Freeing the memory allocated for the data
}

int main()
{
    // Preparing the parameters for the message
    const char *message = "Hello, I'm userC sending to userA!";
    size_t dataSize = strlen(message) + 1; // כולל הסימן הסופי '\0'
    uint32_t destID = 1;
    uint32_t srcID = 3;
    // Creating the communication object with the callback function to process the data
    Communication comm(srcID, processData);
    // Starting communication with the server
    comm.start();
    // Sending the message
    comm.sendMessage((void *)message, dataSize, destID, srcID);
    // Running in a time loop to receive messages and handle them
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}