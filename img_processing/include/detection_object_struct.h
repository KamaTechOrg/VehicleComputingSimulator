#ifndef __DETECTION_OBJECT_STRUCT_H__
#define __DETECTION_OBJECT_STRUCT_H__
#include "object_type_enum.h"
#include <opencv2/opencv.hpp>

struct DetectionObject {
    int id;
    ObjectType type;
    float confidence;
    cv::Rect position;
};

#endif  //__DETECTION_OBJECT_STRUCT_H__