#ifndef __DISTANCE_H__
#define __DISTANCE_H__
#define PERSON_HEIGHT 1700
#define CAR_WIDTH 2000

#include "object_information_struct.h"
#include "log_manager.h"

class Distance {
   public:
    void findDistance(std::vector<ObjectInformation> &objectInformation);
    void setFocalLength(const cv::Mat &image);
    void setFocalLength(double focalLength);
    void drawDistance(std::shared_ptr<cv::Mat> image,
                      std::vector<ObjectInformation> &objects);

   private:
    double focalLength;
    void findFocalLength(const cv::Mat &image);
    void addDistance(float distance, ObjectInformation &obj);
};

#endif  //__DISTANCE_H__