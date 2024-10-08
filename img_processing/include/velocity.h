#ifndef __VELOCITY_H__
#define __VELOCITY_H__

#include <opencv2/opencv.hpp>
#include "object_information_struct.h"

class Velocity {
   public:
    Velocity() {}
    void returnVelocities(std::vector<ObjectInformation> &objects);
    void init(double frameTimeDiff);
    void drawVelocity(const std::shared_ptr<cv::Mat> image,
                      const std::vector<ObjectInformation> &objects) const;

   private:
    double frameTimeDiff;
    void calculateVelocity(ObjectInformation &object);
    float averageDistanceChange(ObjectInformation obj) const;
    void updateVelocity(float newVelocity, ObjectInformation &obj);
};
#endif  //__VELOCITY_H__