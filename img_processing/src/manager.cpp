#include "manager.h"
using namespace std;
using namespace cv;

logger Manager::imgLogger("img_processing");

Manager::Manager()
{
    // Constructor implementation
}

void Manager::init()
{
    imgLogger.logMessage(logger::LogLevel::INFO, "Manager::init() start");
    cv::Mat black_image = cv::Mat::zeros(640, 640, CV_8UC3);
    prevFrame = make_shared<Mat>(black_image);
    currentFrame = make_shared<Mat>(black_image);
    bool isCuda = false;
    detector.init(isCuda);
    dynamicTracker.init();
}

void Manager::processing(const Mat &newFrame)
{
    currentFrame = make_shared<Mat>(newFrame);
    // Use CUDA if existing
    bool is_cuda;  //= argc > 1 && strcmp(argv[1], "cuda") == 0;
    // Loading YOLOv5 model using load_net()
    detector.init(is_cuda);
    // TODO :call to findDifference - A function to find the difference between
    // two frames This function will happen to detect for any change it find
    detector.detect(this->currentFrame);
    this->currentDetectionOutput = detector.getOutput();
    alerter.sendAlerts(this->currentDetectionOutput);
    dynamicTracker.track(this->prevFrame, this->currentFrame,
                         this->prevDetectionOutput,
                         this->currentDetectionOutput);
    // alerter.sendAlerts(dynamicTracker.getOutput());
    prepareForTheNext();
}

void Manager::prepareForTheNext()
{
    this->prevFrame = this->currentFrame;
    this->prevDetectionOutput = this->currentDetectionOutput;
}
