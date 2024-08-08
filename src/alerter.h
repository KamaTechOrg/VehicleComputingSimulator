#ifndef __ALERTER_H__
#define __ALERTER_H__
#include "detection_object_struct.h"

class Alerter {
   private:
    /* data */

   public:
    char *makeAlertBuffer(const DetectionObject& detectionObject);
    void sendAlerts(const std::vector<DetectionObject>& output);
};

#endif  //__ALERTER_H__