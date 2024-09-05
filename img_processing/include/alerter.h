#ifndef __ALERTER_H__
#define __ALERTER_H__
#include "detection_object_struct.h"

class Alerter {
   public:
    // TODO : pass all structs to external file
    void sendAlerts(const std::vector<DetectionObject> &output);
    void makeFileJSON();
   private:
    char *makeAlertBuffer(const DetectionObject &detectionObject);
    void destroyAlertBuffer(char *buffer);
};

#endif  //__ALERTER_H__