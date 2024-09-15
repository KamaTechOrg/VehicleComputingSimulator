#ifndef __MANAGER_H__
#define __MANAGER_H__
#include "alerter.h"
#include "detector.h"
#include "dynamic_tracker.h"
#include "logger.h"
#include <opencv2/opencv.hpp>
class Manager {
public:
    static logger imgLogger;
    // Gets a first image and initializes the class's members
    Manager();
    // Gets the currentFrame and sends it for detection and then tracking,
    // finally if necessary sends a alert
    void processing(const cv::Mat &newFrame);
    void init();

private:
    std::shared_ptr<cv::Mat> prevFrame;
    std::shared_ptr<cv::Mat> currentFrame;
    std::vector<DetectionObject> prevDetectionOutput;
    std::vector<DetectionObject> currentDetectionOutput;
    Detector detector;
    DynamicTracker dynamicTracker;
    Alerter alerter;
    // Moves the current image to the prevFrame and clears the memory of the
    // currentFrame;
    void prepareForTheNext();
};

#endif  //__MANAGER_H__