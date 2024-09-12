#include "../include/alert.h"
using namespace std;

Alert::Alert(){};

Alert::Alert(bool messageType, int level, ObjectType objectType,
             double distance, int carSpeed, int objectSpeed)
    : alertDetails{messageType, level, (int)objectType},
      objectDistance(distance),
      carSpeed(carSpeed),
      objectSpeed(objectSpeed){};

int Alert::getMessageType() const
{
    return alertDetails.messageType;
};

int Alert::getObjectType() const
{
    return alertDetails.objectType;
};

int Alert::getLevel() const
{
    return alertDetails.level;
};

double Alert::getObjectDistance() const
{
    return objectDistance;
};

int Alert::getCarSpeed() const
{
    return carSpeed;
};

int Alert::getObjectSpeed() const
{
    return objectSpeed;
};

vector<char> Alert::serialize()
{
    vector<char> buffer;
    // determine the size of the buffer
    if (alertDetails.messageType) {  // it smart allert
        buffer.reserve(sizeof(AlertDetails) + sizeof(double) + sizeof(int) * 2);
    }
    else {  // it simple alert
        buffer.reserve(sizeof(AlertDetails) + sizeof(double));
    }
    // serialize alertDetails
    char *alertDetailsPtr = reinterpret_cast<char *>(&alertDetails);
    buffer.insert(buffer.end(), alertDetailsPtr,
                  alertDetailsPtr + sizeof(AlertDetails));

    // serialize distance
    char *distancePtr = reinterpret_cast<char *>(&objectDistance);
    buffer.insert(buffer.end(), distancePtr, distancePtr + sizeof(double));

    if (alertDetails.messageType) {  // it smart allert
        // serialize carSpeed
        char *carSpeedPtr = reinterpret_cast<char *>(&carSpeed);
        buffer.insert(buffer.end(), carSpeedPtr, carSpeedPtr + sizeof(int));
        // serialize objectSpeed
        char *objectSpeedPtr = reinterpret_cast<char *>(&objectSpeed);
        buffer.insert(buffer.end(), objectSpeedPtr,
                      objectSpeedPtr + sizeof(int));
    }

    return buffer;
};

void Alert::deserialize(const char *buffer)
{
    int place = 0;
    // Deserialize alertDetails
    memcpy(&alertDetails, buffer, sizeof(AlertDetails));
    place += sizeof(AlertDetails);
    // Deserialize distance
    memcpy(&objectDistance, buffer + place, sizeof(double));
    place += sizeof(double);
    if (alertDetails.messageType) {  // it smart allert
        // Deserialize carSpeed
        memcpy(&carSpeed, buffer + place, sizeof(int));
        place += sizeof(int);
        // Deserialize objectSpeed
        memcpy(&objectSpeed, buffer + place, sizeof(int));
    }
    else {
        carSpeed = objectSpeed = 0;
    }
};