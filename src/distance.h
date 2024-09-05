#ifndef __DISTANCE_H__
#define __DISTANCE_H__
#define PERSON_WIDTH 32
#define CAR_WIDTH 190
#define FOCAL_LENGTH 3.5
#include "detection_object_struct.h"

class Distance{
public:
    double findDistance(const DetectionObject& detectionObject);
private:
    /* data */
};

#endif  //__DISTANCE_H__