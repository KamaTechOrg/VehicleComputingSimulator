#include "manager.h"
#include <chrono>
#include <opencv2/opencv.hpp>
#include <thread>

using namespace std;
using namespace cv;

Mat frame;
void deserializeData(void *data)
{
    // if (data == nullptr) {
    //     Manager::imgLogger.logMessage(logger::LogLevel::ERROR,
    //                                   "Error: Received null data pointer");
    //     return;
    // }
    // Manager::imgLogger.logMessage(
    //     logger::LogLevel::INFO,
    //     "Received data: " + string(static_cast<char *>(data)));
    free(data);
    frame = imread("../tests/images/track_2_cars_first_frame.jpg");
    if (frame.empty()) {
        Manager::imgLogger.logMessage(logger::LogLevel::ERROR,
                                      "Error: Could not read the image file");
        return;
    }
}

int main()
{
    // Preparing the parameters for the message
    const char *message = "Hello, I'm userA sending to userB!";
    size_t dataSize = strlen(message) + 1;
    uint32_t destID = 2;
    uint32_t srcID = 1;
    // Creating the communication object with t

    // Running in a time loop to receive messages and handle them
    Manager manager;
    manager.init();

    void *data = nullptr;
    deserializeData(data);
    manager.processing(frame);
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}