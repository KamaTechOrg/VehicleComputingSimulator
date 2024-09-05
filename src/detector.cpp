#include "detector.h"
using namespace std;
using namespace cv;
using namespace dnn;

void Detector::detect(const shared_ptr<Mat>& frame)
{
    //intialize variables
    output.clear();
    this->prevFrame = this->currentFrame;
    this->currentFrame = frame;
    if (!prevFrame) {
        detectObjects(currentFrame, Point(0, 0));
    }
    else {
        detectChanges();
    }
}

void Detector::detectObjects(const shared_ptr<Mat>& frame, const Point& position)
{
    //intialize variables
    output.clear();
    this->prevFrame = this->currentFrame;
    this->currentFrame = frame;
    if (!prevFrame) {
        detectObjects(currentFrame, Point(0, 0));
    }
    else {
        detectChanges();
    }
}

void Detector::detectObjects(const shared_ptr<Mat>& frame, const Point& position)
{
    /*
    Performs object detection on a video media using the YOLOv5 model
    Converts the input images to a blob suitable for model input
    Processes the model outputs to extract class IDs,
    confidences, and bounding box coordinates
    Applies confidence and threshold filtering and performs
    non-maximum suppression to eliminate redundant detections
    */
    Mat blob;
    auto inputImage = formatYolov5(frame);
    blobFromImage(inputImage, blob, 1. / 255., Size(INPUT_WIDTH, INPUT_HEIGHT),
                  Scalar(), true, false);
    net.setInput(blob);
    vector<Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());
    float xFactor = inputImage.cols / INPUT_WIDTH;
    float yFactor = inputImage.rows / INPUT_HEIGHT;
    float *data = (float *)outputs[0].data;
    // const int dimensions = 85;
    const int dimensions = 7;
    const int rows = 25200;
    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;
    for (int i = 0; i < rows; ++i) {
        float confidence = data[4];
        if (confidence >= CONFIDENCE_THRESHOLD) {
            float *classesScores = data + 5;
            Mat scores(1, ObjectType::COUNT, CV_32FC1, classesScores);
            Point classId;
            double maxClassScore;
            minMaxLoc(scores, 0, &maxClassScore, 0, &classId);
            if (maxClassScore > SCORE_THRESHOLD &&
                isValidObjectType(classId.x)) {
                confidences.push_back(confidence);
                classIds.push_back(classId.x);
                float x = data[0];
                float y = data[1];
                float w = data[2];
                float h = data[3];
                // Width, height and x,y coordinates of bounding box
                int left = int((x - 0.5 * w) * xFactor);
                int top = int((y - 0.5 * h) * yFactor);
                int width = int(w * xFactor);
                int height = int(h * yFactor);
                boxes.push_back(Rect(left, top, width, height));
            }
        }
        data += 85;
    }
    vector<int> nmsResult;
    NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, nmsResult);
    for (int i = 0; i < nmsResult.size(); i++) {
        int idx = nmsResult[i];
        DetectionObject result;
        // The conversion may fail because the model is trained to identify different objects
        // A model may be identified with a number greater than 2
        // While ObjectType Only keeps 3 organs
        result.type = static_cast<ObjectType>(classIds[idx]);
        result.confidence = confidences[idx];
        result.position = boxes[idx];
        output.push_back(result);
    }
}

void Detector::detectChanges()
{
    const vector<Rect> changedAreas = findDifference();
    cout << "changedAreas:" << changedAreas.size() << endl;
    //TODO : delete all rects that contained in another rect
    for (Rect oneChange : changedAreas) {
        int x = oneChange.x;
        int y = oneChange.y;
        Point position(x, y);
        Mat view(*currentFrame, oneChange);
        detectObjects(make_shared<Mat>(view), position);
    }
}

vector<Rect> Detector::findDifference()
{
    vector<Rect> differencesRects;
    Mat prevGray, currentGray;
    cvtColor(*prevFrame, prevGray, COLOR_BGR2GRAY);
    cvtColor(*currentFrame, currentGray, COLOR_BGR2GRAY);
    // Find the difference between the two images
    Mat diff;
    absdiff(prevGray, currentGray, diff);
    // Apply threshold
    Mat thresh;
    threshold(diff, thresh, 0, 255, THRESH_BINARY | THRESH_OTSU);
    // Dilation
    Mat kernel = Mat::ones(5, 5, CV_8U);
    Mat dil;
    dilate(thresh, dil, kernel, Point(-1, -1), 2);
    // Calculate contours
    std::vector<std::vector<Point>> contours;
    findContours(dil, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for (const auto &contour : contours) {
        // Calculate bounding box around contour
        Rect boundingBox = boundingRect(contour);
        differencesRects.push_back(boundingBox);
    }
    cout << "num of difference:" << differencesRects.size() << endl;
    vector<Rect> unionRects = unionOverlappingRectangels(differencesRects);
    cout << "after union:" << unionRects.size() << endl;
    return unionRects;
}

vector<Rect> Detector::unionOverlappingRectangels(vector<Rect> allChanges)
{
    vector<bool> isValid(allChanges.size(), true);
    for (int i = 0; i < allChanges.size(); i++) {
        if (isValid[i]) {
            for (int j = 0; j < allChanges.size(); j++) {
                if (isValid[j]) {
                    if (i != j && (allChanges[i] & allChanges[j]).area() > 0) {
                        if (i > j) {
                            allChanges[i] |= allChanges[j];
                            isValid[j] = false;
                        }
                        else {
                            allChanges[j] |= allChanges[i];
                            isValid[i] = false;
                        }
                    }
                }
            }
        }
    }
    for (int i = 0; i < allChanges.size(); i++) {
        if (!isValid[i]) {
            isValid.erase(isValid.begin() + i);
            allChanges.erase(allChanges.begin() + i);
            i--;
        }
    }
    return allChanges;
}

vector<DetectionObject> Detector::getOutput() const
{
    return output;
}

Mat Detector::formatYolov5(const shared_ptr<Mat>& frame)
{
    int col = frame->cols;
    int row = frame->rows;
    int maximum = MAX(col, row);
    Mat result = Mat::zeros(maximum, maximum, CV_8UC3);
    frame->copyTo(result(Rect(0, 0, col, row)));
    return result;
}

void Detector::init(bool isCuda)
{
    loadNet(isCuda);
}

void Detector::loadNet(bool isCuda)
{
    // Loading yolov5s onnx model
    auto result = readNet("../yolov5s.onnx");
    if (isCuda) {
        cout << "Using CUDA\n";
        result.setPreferableBackend(DNN_BACKEND_CUDA);
        result.setPreferableTarget(DNN_TARGET_CUDA_FP16);
    }
    else {
        cout << "CPU Mode\n";
        result.setPreferableBackend(DNN_BACKEND_OPENCV);
        result.setPreferableTarget(DNN_TARGET_CPU);
    }
    net = result;
}

bool Detector::isValidObjectType(int value) const
{
    switch (value) {
        case PEOPLE:
        case CAR:
            return true;
        default:
            return false;
    }
}
