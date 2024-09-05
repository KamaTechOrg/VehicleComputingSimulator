#include <opencv2/opencv.hpp>
#include "distance.h"

double Distance::findDistance(const DetectionObject& detectionObject)
{
    int knowWidth;
    if(detectionObject.type==0){
        knowWidth=PERSON_WIDTH;
    }
    else{
        knowWidth=CAR_WIDTH;
    }
    std::cout <<"width object in the img: "<< detectionObject.position.width<< std::endl;
    std::cout <<"width realy: "<< knowWidth << std::endl;
    std::cout <<"focalLength: "<< FOCAL_LENGTH << std::endl;
    return FOCAL_LENGTH*knowWidth/(detectionObject.position.width);
}


