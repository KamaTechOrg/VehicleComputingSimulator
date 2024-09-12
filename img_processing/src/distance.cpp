#include <opencv2/opencv.hpp>
#include "../include/distance.h"
#include "../include/detector.h"

Distance::Distance()
{
    findFocalLength();
}

double Distance::findDistance(const DetectionObject &detectionObject)
{
    int knowWidth;
    if (detectionObject.type == 0) {
        knowWidth = PERSON_WIDTH;
    }
    else {
        knowWidth = CAR_WIDTH;
    }
    std::cout << "width object in the img: " << detectionObject.position.width
              << std::endl;
    std::cout << "width realy: " << knowWidth << std::endl;
    std::cout << "focalLength: " << this->focalLength << std::endl;
    return this->focalLength * knowWidth / detectionObject.position.width;
}

void Distance::findFocalLength()
{
    // Load a real image from file
    std::string imagePath = "../tests/images/IMG_0057.JPG";
    cv::Mat testImage;
    testImage = cv::imread(imagePath);
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

    double imgWidth = output[0].position.width;

    this->focalLength = imgWidth * 1450 / 360;
}
