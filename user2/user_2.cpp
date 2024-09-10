#include <iostream>
#include "../src/communication.h"

// Processing the data received from the complete message
void processData(void *data)
{
    std::cout << "Received data: " << static_cast<char *>(data) << std::endl;
    free(data); 
}

int main()
{
    // Preparing the parameters for the message
    const char *message = "Hello, I'm userB sending to userA!";
    size_t dataSize = strlen(message) + 1; 
    uint32_t destID = 1;
    uint32_t srcID = 2;
    // Creating the communication object with the callback function to process the data
    Communication comm(srcID, processData);
    // Starting communication with the server
    comm.startConnection();
    // Sending the message
    comm.sendMessage((void *)message, dataSize, destID, srcID,false);
    // Running in a time loop to receive messages and handle them
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}