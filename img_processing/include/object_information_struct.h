#ifndef __TRACKER_OBJECT_STRUCT_H__
#define __TRACKER_OBJECT_STRUCT_H__
#include <opencv2/opencv.hpp>
#include "object_type_enum.h"

struct ObjectInformation {
    int id;
    ObjectType type;
    cv::Rect prevPosition;
    cv::Rect position;
    double prevDistance;
    double distance = -1.0;
    double prevVelocity;
    double velocity;
};

#endif  //__TRACKER_OBJECT_STRUCT_H__