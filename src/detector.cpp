#include "detector.h"
using namespace std;
using namespace cv;
using namespace dnn;

void Detector::detect(const shared_ptr<Mat> &frame) {
  /*
  Performs object detection on a video media using the YOLOv5 model
  Converts the input images to a blob suitable for model input
  Processes the model outputs to extract class IDs,
  confidences, and bounding box coordinates
  Applies confidence and threshold filtering and performs
  non-maximum suppression to eliminate redundant detections
  */
  this->frame = frame;
  Mat blob;
  auto inputImage = formatYolov5();
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
      if (maxClassScore > SCORE_THRESHOLD && isValidObjectType(classId.x)) {
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
    // The conversion may fail because the model is trained to identify
    // different objects A model may be identified with a number greater than 2
    // While ObjectType Only keeps 3 organs
    result.type = static_cast<ObjectType>(classIds[idx]);
    result.confidence = confidences[idx];
    result.position = boxes[idx];
    output.push_back(result);
  }
}

vector<DetectionObject> Detector::getOutput() const { return output; }

Mat Detector::formatYolov5() {
  int col = frame->cols;
  int row = frame->rows;
  int maximum = MAX(col, row);
  Mat result = Mat::zeros(maximum, maximum, CV_8UC3);
  frame->copyTo(result(Rect(0, 0, col, row)));
  return result;
}

void Detector::init(bool isCuda) { loadNet(isCuda); }

void Detector::loadNet(bool isCuda) {
  // Loading yolov5s onnx model
  auto result = readNet("../yolov5s.onnx");
  if (isCuda) {
    cout << "Using CUDA\n";
    result.setPreferableBackend(DNN_BACKEND_CUDA);
    result.setPreferableTarget(DNN_TARGET_CUDA_FP16);
  } else {
    cout << "CPU Mode\n";
    result.setPreferableBackend(DNN_BACKEND_OPENCV);
    result.setPreferableTarget(DNN_TARGET_CPU);
  }
  net = result;
}

bool Detector::isValidObjectType(int value) const {
  switch (value) {
  case PEOPLE:
  case CAR:
    return true;
  default:
    return false;
  }
}
