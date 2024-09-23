#include "../include/manager.h"
#include "../include/alert.h"

using namespace std;
using namespace cv;
#define NUM_OF_TRACKING 10

logger Manager::imgLogger("img_processing");

void Manager::init()
{
    Mat calibrationImage = imread("../tests/images/black_line.JPG");
    if (calibrationImage.empty()) {
        Manager::imgLogger.logMessage(
            logger::LogLevel::ERROR,
            "image not found");
        return;
    }
    Distance &distance = Distance::getInstance(calibrationImage);
    iterationCnt = 0;
    bool isCuda = false;
    detector.init(isCuda);
    dynamicTracker.init();
}

void Manager::mainDemo()
{
    VideoCapture capture("../tests/images/close_cars.mov");
    Mat frame = Mat::zeros(480, 640, CV_8UC3);
    if (!capture.isOpened()) {
        Manager::imgLogger.logMessage(
            logger::LogLevel::ERROR,
            "Error while opening video media");
        return;
    }
    while (1) {
        capture >> frame;
        if (frame.empty()) {
            Manager::imgLogger.logMessage(
                logger::LogLevel::INFO,
                "media finish");
            break;
        }
        int result = processing(frame, true);
        if (result == -1)
            return;
    }
}

bool Manager::isDetect(bool isTravel)
{
    if (!isTravel || iterationCnt == 0)
        return true;
    return false;
}

bool Manager::isResetTracker(bool isTravel)
{
    if (isTravel && iterationCnt == 0)
        return true;
    return false;
}

bool Manager::isTrack(bool isTravel)
{
    if (isTravel && iterationCnt > 0)
        return true;
    return false;
}

int Manager::processing(const Mat &newFrame, bool isTravel)
{
    Distance &distance = Distance::getInstance();
    currentFrame = make_shared<Mat>(newFrame);
    if (isDetect(isTravel)) {
        //send the frame to detect
        detector.detect(this->currentFrame, isTravel);
        this->currentOutput = detector.getOutput();
    }

    if (isResetTracker(isTravel)) {
        //prepare the tracker
        dynamicTracker.startTracking(this->currentFrame, this->currentOutput);
    }

    if (isTrack(isTravel)) {
        //send the frame to track
        dynamicTracker.tracking(this->currentFrame,this->currentOutput);
    }

    //add distance to detection objects
    distance.findDistance(this->currentOutput);

    //send allerts to main control
    vector<unique_ptr<char>> alerts = alerter.sendAlerts(this->currentOutput);
    sendAlerts(alerts);

    // update of the iterationCnt
    if (isTravel) {
        iterationCnt = iterationCnt == NUM_OF_TRACKING ? 0 : iterationCnt + 1;
    }

    //visual
    drowOutput();
    imshow("aaa", *currentFrame);
    int key = cv::waitKey(1);
    if (key == 27) {
        return -1;
    }
    return 1;
}

void Manager::drowOutput() {
    for (ObjectInformation objectInformation : currentOutput) {
        int topLeftX = objectInformation.position.x;
        int topLeftY = objectInformation.position.y;

        // Draw rectangle around object
        Scalar boxColor = (objectInformation.distance <(alerter.MIN_LEGAL_DISTANCE) ) ? Scalar(0, 0, 255) : Scalar(0, 255, 0);
        rectangle(*currentFrame, objectInformation.position, boxColor, 2);

        // Define text for distance and speed
        std::stringstream ssDistance, ssSpeed;
        ssDistance << std::fixed << std::setprecision(2) << objectInformation.distance;
        ssSpeed << std::fixed << std::setprecision(2) << 0;//velothity;

        std::string distanceText = ssDistance.str();
        std::string speedText = ssSpeed.str();

        // Font properties
        int fontFace = FONT_HERSHEY_SIMPLEX;
        double fontScale = 0.6;
        int thickness = 1;
        int baseline = 0;

        // Calculate text sizes
        Size distanceTextSize = getTextSize(distanceText, fontFace, fontScale, thickness, &baseline);
        Size speedTextSize = getTextSize(speedText, fontFace, fontScale, thickness, &baseline);

        // Positions for the texts
        Point distanceTextOrg(topLeftX + 5, topLeftY - speedTextSize.height - 7);  // Above the object
        Point speedTextOrg(topLeftX + 5, topLeftY - 5);  // Above the object

        // Draw outline for distance text
        putText(*currentFrame, distanceText, distanceTextOrg, fontFace, fontScale, Scalar(0, 0, 0), thickness + 2);
        // Write the distance text
        putText(*currentFrame, distanceText, distanceTextOrg, fontFace, fontScale, Scalar(255, 255, 255), thickness);
        
        // Draw outline for speed text
        putText(*currentFrame, speedText, speedTextOrg, fontFace, fontScale, Scalar(0, 0, 0), thickness + 2);
        // Write the speed text
        putText(*currentFrame, speedText, speedTextOrg, fontFace, fontScale, Scalar(255, 0, 0), thickness);
    }

    // Legend
    int legendX = 10, legendY = 10;
    putText(*currentFrame, "Legend:", Point(legendX, legendY), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 1);
    rectangle(*currentFrame, Point(legendX, legendY + 10), Point(legendX + 10, legendY + 30), Scalar(255, 255, 255), FILLED);
    putText(*currentFrame, "Distance", Point(legendX + 15, legendY + 25), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 1);
    rectangle(*currentFrame, Point(legendX, legendY + 35), Point(legendX + 10, legendY + 55), Scalar(255, 0, 0), FILLED);
    putText(*currentFrame, "Speed", Point(legendX + 15, legendY + 50), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 1);
}

void Manager::sendAlerts(vector<unique_ptr<char>> &alerts)
{
    for (const std::unique_ptr<char> &alertBuffer : alerts) {
        //use send communication function
    }
}

void Manager::prepareForTheNext()
{
    prevFrame = currentFrame;
}