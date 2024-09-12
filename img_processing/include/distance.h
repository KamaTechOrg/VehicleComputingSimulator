#ifndef __DISTANCE_H__
#define __DISTANCE_H__
#include "detection_object_struct.h"
#define PERSON_HEIGHT 1700
#define CAR_WIDTH 2000

class Distance {
   public:
    void findDistance(std::vector<DetectionObject> &detectionObjects);
    static Distance &getInstance(const cv::Mat &image = cv::Mat());

   private:
    static Distance *instance;
    double focalLength;
    Distance(const cv::Mat &image);
    Distance(const Distance &) = delete;
    Distance &operator=(const Distance &) = delete;
    void findFocalLength(const cv::Mat &image);
};

#endif  //__DISTANCE_H__