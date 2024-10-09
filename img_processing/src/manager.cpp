#define NUM_OF_TRACKING 10

#include "manager.h"
#include "alert.h"

using namespace std;
using namespace cv;

void processData(uint32_t srcId, void *data) {}

Manager::Manager(int processID)
    : processID(processID), communication(processID, processData)
{
}

void Manager::init()
{
    string message = "Hello, I'm img_processing " + to_string(processID) +
                     " sending to process " + to_string(destID);
    size_t dataSize = message.length() + 1;
    destID = 1;
    // Starting communication with the server
    communication.startConnection();
    // Sending the message
    communication.sendMessage((void *)message.c_str(), dataSize, destID,
                              processID, false);

    // calibration
    Mat calibrationImage = imread("../tests/images/black_line.JPG");
    if (calibrationImage.empty()) {
        LogManager::logErrorMessage(ErrorType::IMAGE_ERROR, "image not found");
        return;
    }
    distance.setFocalLength(calibrationImage);
    iterationCnt = 1;
    bool isCuda = false;
    detector.init(isCuda);
    dynamicTracker.init();
    velocity.init(0.04);
}

void Manager::mainDemo()
{
    string filePath = "../data.txt";
    // open the file
    ifstream file(filePath);
    if (!file.is_open()) {
        LogManager::logErrorMessage(ErrorType::FILE_ERROR);
        return;
    }
    string line;
    // run over the file and read the lines
    while (getline(file, line)) {
        // intialize the iteration cnt
        iterationCnt = 1;
        istringstream iss(line);
        string videoPath;
        double focalLength;
        // read the parameters
        if (getline(iss, videoPath, '|') && iss >> focalLength) {
            // Trim leading and trailing whitespaces from videoPath
            videoPath.erase(0, videoPath.find_first_not_of(" \t\n\r\f\v"));
            videoPath.erase(videoPath.find_last_not_of(" \t\n\r\f\v") + 1);
        }
        else {
            LogManager::logErrorMessage(ErrorType::VIDEO_ERROR);
            return;
        }
        // intialize focal length
        distance.setFocalLength(focalLength);
        runOnVideo(videoPath);
    }
    cout << "finish reading data";
}

void Manager::runOnVideo(string videoPath)
{
    // Convert Windows file path to WSL file path format
    if (videoPath.length() >= 3 && videoPath[1] == ':') {
        // Convert to lowercase
        char driveLetter = tolower(static_cast<unsigned char>(videoPath[0]));
        videoPath = "/mnt/" + string(1, driveLetter) + videoPath.substr(2);
        // Replace backslashes with forward slashes
        replace(videoPath.begin(), videoPath.end(), '\\', '/');
    }
    // open the video
    VideoCapture capture(videoPath);
    Mat frame = Mat::zeros(480, 640, CV_8UC3);
    if (!capture.isOpened()) {
        LogManager::logErrorMessage(ErrorType::VIDEO_ERROR, "video not found");
        throw runtime_error("video not found");
        return;
    }
    while (1) {
        capture >> frame;
        if (frame.empty()) {
            LogManager::logInfoMessage(InfoType::MEDIA_FINISH);
            break;
        }
        int result = processing(frame, true);
        if (result == -1)
            return;
    }
}

bool Manager::isDetect()
{
    if (!isTravel || iterationCnt == 1)
        return true;
    return false;
}

bool Manager::isResetTracker()
{
    if (isTravel && iterationCnt == 1)
        return true;
    return false;
}

bool Manager::isTrack()
{
    if (isTravel && iterationCnt > 1)
        return true;
    return false;
}

bool Manager::isCalcVelocity()
{
    if (isTravel && iterationCnt > 1)
        return true;
    return false;
}

int Manager::processing(const Mat &newFrame, bool isTravel)
{
    this->isTravel = isTravel;
    currentFrame = make_shared<Mat>(newFrame);
    if (isDetect()) {
        // send the frame to detect
        detector.detect(this->currentFrame, isTravel);
        this->currentOutput = detector.getOutput();
    }

    if (isResetTracker()) {
        // prepare the tracker
        dynamicTracker.startTracking(this->currentFrame, this->currentOutput);
    }

    if (isTrack()) {
        // send the frame to track
        dynamicTracker.tracking(this->currentFrame, this->currentOutput);
    }

    // add distance to detection objects
    distance.findDistance(this->currentOutput);
    if (isCalcVelocity()) {
        velocity.returnVelocities(this->currentOutput);
    }

    // send allerts to main control
    if (isCalcVelocity()) {
        vector<vector<uint8_t>> alerts =
            alerter.sendAlerts(this->currentOutput);
        sendAlerts(alerts);
    }

    // update of the iterationCnt
    if (isTravel) {
        iterationCnt = iterationCnt % NUM_OF_TRACKING + 1;
    }

    // visual
    if (drawOutput() == 27)
        return -1;
    return 1;
}

int Manager::drawOutput()
{
    dynamicTracker.drawTracking(currentFrame, currentOutput);
    distance.drawDistance(currentFrame, currentOutput);
    if (isCalcVelocity())
        velocity.drawVelocity(currentFrame, currentOutput);

    // Legend
    int legendX = 10, legendY = 10;

    //Draw a black border around the legend
    rectangle(*currentFrame, Point(legendX - 10, legendY - 10),
              Point(legendX + 162, legendY + 72), Scalar(0, 0, 0), 2);

    // Draw a black rectangle as background for the legend
    rectangle(*currentFrame, Point(legendX - 8, legendY - 8),
              Point(legendX + 160, legendY + 70), Scalar(150, 150, 150),
              FILLED);

    // Draw the legend text and colors
    putText(*currentFrame, "Legend:", Point(legendX, legendY + 7),
            FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 1);
    rectangle(*currentFrame, Point(legendX, legendY + 17),
              Point(legendX + 10, legendY + 37), Scalar(255, 255, 255), FILLED);
    putText(*currentFrame, "Distance", Point(legendX + 15, legendY + 37),
            FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 1);
    rectangle(*currentFrame, Point(legendX, legendY + 47),
              Point(legendX + 10, legendY + 62), Scalar(255, 255, 0), FILLED);
    putText(*currentFrame, "Velocity", Point(legendX + 15, legendY + 62),
            FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 1);

    imshow("Output", *currentFrame);
    int key = waitKey(1);
    return key;
}

void Manager::sendAlerts(vector<vector<uint8_t>> &alerts)
{
    for (std::vector<uint8_t> &alertBuffer : alerts) {
        communication.sendMessage(alertBuffer.data(), alertBuffer.size(),
                                  destID, processID, false);
    }
}

void Manager::prepareForTheNext()
{
    prevFrame = currentFrame;
}