#ifndef __DYNAMIC_TRACKER_H__
#define __DYNAMIC_TRACKER_H__
#include "detection_object_struct.h"
#include "object_type_enum.h"
#include "tracker_object_struct.h"
#include <opencv2/opencv.hpp>

class DynamicTracker {
private:
    std::shared_ptr<cv::Mat> prevFrame;
    std::shared_ptr<cv::Mat> currentFrame;
    std::vector<TrackerObject> output;

public:
    // loading the moodle
    void init();
    void track(const std::shared_ptr<cv::Mat> &prevFrame,
               const std::shared_ptr<cv::Mat> &currentFrame,
               const std::vector<DetectionObject> &prevOutput,
               const std::vector<DetectionObject> &currentOutput);
    std::vector<TrackerObject> getOutput() const;
};

#endif  // __DYNAMIC_TRACKER_H__
