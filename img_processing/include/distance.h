#ifndef __DISTANCE_H__
#define __DISTANCE_H__
#define PERSON_WIDTH 330
#define CAR_WIDTH 2000
#include "detection_object_struct.h"

class Distance {
   public:
    double findDistance(const DetectionObject &detectionObject);
    Distance();

   private:
    double focalLength;
    void findFocalLength();
};

#endif  //__DISTANCE_H__