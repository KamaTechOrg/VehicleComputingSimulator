#ifndef __OBJECT_TYPE_ENUM_H__
#define __OBJECT_TYPE_ENUM_H__

// the integer values acording to the order that yolov5 model recognize them
// The numbers of the objects match according to the model
enum ObjectType {
    PEOPLE = 0,
    BICYCLE = 1,
    CAR = 2,
    MOTORBIKE = 3,
    BUS = 5,
    TRAIN = 6,
    TRUCK = 7,
    COUNT
};

#endif  // __OBJECT_TYPE_ENUM_H__
