#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>
#include "../include/distance.h"
#include "../include/detector.h"
#include "manager.h"

using namespace std;
using namespace cv;
#define MIN_LEGAL_HEIGHT 900

Distance *Distance::instance = nullptr;

Distance::Distance(const cv::Mat &image)
{
    findFocalLength(image);
}

Distance &Distance::getInstance(const cv::Mat &image)
{
    if (!instance) {
        if (image.empty())
            Manager::imgLogger.logMessage(
            logger::LogLevel::ERROR,
            "No image was provided");
        else
            instance = new Distance(image);
    }
    return *instance;
}

// function that finds the distance of the objects and update them in detectionObjects
void Distance::findDistance(std::vector<ObjectInformation> &objectInformations)
{
    int knownSize, imageSize;
    // Move over all objects
    for (ObjectInformation &objectInformation : objectInformations) {
        // is person
        if (objectInformation.type == 0) {
            // distance test based on the position of the legs in the image
            if (objectInformation.position.y +
                    objectInformation.position.height >
                MIN_LEGAL_HEIGHT) {
                objectInformation.distance = 0;
                return;
            }
            // Find the size of the object in reality and in the picture
            knownSize = PERSON_HEIGHT;
            imageSize = objectInformation.position.height;
        }
        else {
            // Find the size of the object in reality and in the picture
            knownSize = CAR_WIDTH;
            imageSize = objectInformation.position.width;
        }
        // Calculate the distance in meters
        objectInformation.distance =
            (focalLength * knownSize / imageSize) / 1000;
    }
}

void Distance::findFocalLength(const cv::Mat &image)
{
    // Check if the input image is empty
    if (image.empty()) {
        Manager::imgLogger.logMessage(
            logger::LogLevel::ERROR,
            "Could not open or find the image");
            return;
            //throw std::runtime_error("Could not open or find the image");
    }

    // Convert the input image to grayscale
    Mat testImage, grayImage;
    image.copyTo(testImage);
    cvtColor(testImage, grayImage, COLOR_BGR2GRAY);
    grayImage = 255 - grayImage;

    // Apply thresholding to the grayscale image
    Mat thresh;
    threshold(grayImage, thresh, 0, 255, THRESH_BINARY | THRESH_OTSU);

    // Find contours in the thresholded image
    std::vector<std::vector<Point>> contours;
    findContours(thresh, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // Find the longest contour (assumed to be the black line)
    double maxLength = 0.0;
    std::vector<cv::Point> maxContour;
    for (const auto &contour : contours) {
        double length = cv::arcLength(contour, true);
        if (length > maxLength) {
            maxLength = length;
            maxContour = contour;
        }
    }

    // Find the top-left and bottom-right points of the bounding box
    cv::Point topLeft = maxContour[0];
    cv::Point bottomRight = maxContour[0];
    for (const auto &point : maxContour) {
        if (point.y < topLeft.y ||
            (point.y == topLeft.y && point.x < topLeft.x)) {
            topLeft = point;
        }
        if (point.y > bottomRight.y ||
            (point.y == bottomRight.y && point.x > bottomRight.x)) {
            bottomRight = point;
        }
    }

    // Calculate the width of the bounding box
    double rectWidth = sqrt(pow(bottomRight.y - topLeft.y, 2) +
                            pow(bottomRight.x - topLeft.x, 2));

    // Known parameters
    const double actualLineLengthMm = 200.0;
    const double distanceToCameraMm = 1000.0;

    // Calculate focal length in pixels
    this->focalLength = (rectWidth * distanceToCameraMm) / actualLineLengthMm;
}