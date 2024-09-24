#ifndef __TRACKER_OBJECT_STRUCT_H__
#define __TRACKER_OBJECT_STRUCT_H__
#include <opencv2/opencv.hpp>
#include "object_type_enum.h"
#define MAX_PREV_DISTANCES_SIZE 10
struct ObjectInformation {
    int id;
    ObjectType type;
    cv::Rect prevPosition;
    cv::Rect position;
    std::deque<double> prevDistances;
    double distance;
    double velocity;
};

#endif  //__TRACKER_OBJECT_STRUCT_H__