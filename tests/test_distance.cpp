#include <opencv2/opencv.hpp>
#include <gtest/gtest.h>
#include "detector.h"
#include "distance.h"

TEST(DistanceTest, DistanceFromPerson)
{
    // Load a real image from file
    std::string imagePath = "../tests/images/IMG_0058.JPG";
    cv::Mat testImage;
    testImage= cv::imread(imagePath);
    if (testImage.empty()) {
        throw std::runtime_error("Could not open or find the image");
    }
    // Wrap it in a shared_ptr
    std::shared_ptr<cv::Mat> frame = std::make_shared<cv::Mat>(testImage);

    // Create Detector instance
    Detector detector;

    // bool is_cuda = false; // or true, depending on your setup
    detector.init(false);

    // Perform detection
    detector.detect(frame);

    // Get output
    const auto &output = detector.getOutput();

    // Check if output is not empty
    ASSERT_FALSE(output.empty());

    Distance distance;
    
    std::cout << "**************"<<std::endl;
    for (auto detectionObject:output)
    {
        std::cout << distance.findDistance(detectionObject);
    }
}

