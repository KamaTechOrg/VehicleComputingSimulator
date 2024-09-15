#ifndef __DYNAMIC_TRACKER_H__
#define __DYNAMIC_TRACKER_H__
#include <opencv2/opencv.hpp>
#include "detection_object_struct.h"
#include "object_type_enum.h"
class DynamicTracker {
   public:
    // loading the moodle
    void init();
    void startTracking(const std::shared_ptr<cv::Mat> &Frame,
                       const std::vector<DetectionObject> &DetectionOutput);
    std::vector<DetectionObject> getOutput() const;
    void tracking(const std::shared_ptr<cv::Mat> &frame);

   private:
    std::shared_ptr<cv::Mat> frame;
    std::vector<DetectionObject> output;
    std::vector<cv::Ptr<cv::Tracker>> trackers;
    std::vector<int> failedCount;
    int maxFailures = 1;
};

#endif  // __DYNAMIC_TRACKER_H__
