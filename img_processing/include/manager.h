#ifndef __MANAGER_H__
#define __MANAGER_H__
#include <opencv2/opencv.hpp>
#include "alerter.h"
#include "detector.h"
#include "dynamic_tracker.h"
#include "distance.h"
#include "logger.h"
#include "velocity.h"

class Manager {
   public:
    static logger imgLogger;
    Manager() {}
    // Manager(const cv::Mat &Frame);
    // Gets the currentFrame and sends it for detection and then tracking,
    // finally if necessary sends a alert
    int processing(const cv::Mat &newFrame, bool mode);
    void mainDemo();
    //init all variabels and creat the firs instance of distance
    void init();
    void sendAlerts(std::vector<std::unique_ptr<char>> &alerts);

   private:
    std::shared_ptr<cv::Mat> prevFrame;
    std::shared_ptr<cv::Mat> currentFrame;
    std::vector<ObjectInformation> prevOutput;
    std::vector<ObjectInformation> currentOutput;
    Detector detector;
    Velocity velocity;
    DynamicTracker dynamicTracker;
    Alerter alerter;
    //Distance distance;
    int iterationCnt;
    // Moves the current image to the prevFrame and clears the memory of the currentFrame;
    void prepareForTheNext();
    void drowOutput();
    bool isDetect(bool isTravel);
    bool isResetTracker(bool isTravel);
    bool isTrack(bool isTravel);
};
#endif  //__MANAGER_H__
