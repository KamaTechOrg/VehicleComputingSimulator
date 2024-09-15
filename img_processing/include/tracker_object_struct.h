#ifndef __TRACKER_OBJECT_STRUCT_H__
#define __TRACKER_OBJECT_STRUCT_H__
#include "object_type_enum.h"
#include <opencv2/opencv.hpp>

struct TrackerObject {
    int id;
    ObjectType type;
    cv::Rect prevPosition;
    cv::Rect currentPosition;
};

#endif  //__TRACKER_OBJECT_STRUCT_H__