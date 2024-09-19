#ifndef __ALERTER_H__
#define __ALERTER_H__
#include "object_information_struct.h"

class Alerter {
   public:
    std::vector<std::unique_ptr<char>> sendAlerts(
        const std::vector<ObjectInformation> &output);
    void makeFileJSON();

   private:
    char *makeAlertBuffer(int type, double distance);
    bool isSendAlert(const ObjectInformation &objectInformation);
    void destroyAlertBuffer(char *buffer);
};

#endif  //__ALERTER_H__