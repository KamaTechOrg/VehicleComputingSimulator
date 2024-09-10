#include <opencv2/opencv.hpp>
#include "../include/manager.h"

using namespace std;
using namespace cv;

Mat frame;
void deserializeData(void* data){
    std::cout << "Received data: " << static_cast<char *>(data) << std::endl;
    free(data);
    frame = imread("../tests/images/track_2_cars_first_frame.jpg");
    if (frame.empty()) {
        std::cerr << "Error: Could not read the image file." << std::endl;
        return;
    }
}

int main(){
     // Preparing the parameters for the message
    const char *message = "Hello, I'm userA sending to userB!";
    size_t dataSize = strlen(message) + 1; 
    uint32_t destID = 2;
    uint32_t srcID = 1;
    // Creating the communication object with t
    
    // Running in a time loop to receive messages and handle them
    Manager manager;
    manager.init();
    void*data=nullptr;
    deserializeData(data);
    manager.processing(frame) ;
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}