#ifndef __ALERTER_H__
#define __ALERTER_H__
#include "detection_object_struct.h"
// #include "communication.h"

class Alerter {
private:
    /* data */
    // Communication comm;
public:
    Alerter();
    // TODO : pass all structs to external file
    char *makeAlertBuffer(const DetectionObject &detectionObject);
    void sendAlerts(const std::vector<DetectionObject> &output);
    void makeFileJSON();
};

#endif  //__ALERTER_H__