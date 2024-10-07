#ifndef __DATA_DEFINITIONS__
#define __DATA_DEFINITIONS__

#include <Arduino.h>
#include <iostream>
#include <thread>
#include <chrono>

#include "../../../communication/src/communication.h"
#include "../../../logger/logger.h"

// Defines
#define MAX_NORMAL_VALUE 60
#define REASONABLE_GAP 10
#define INVALID_CONTINUITY 3
#define WAIT_TIME 500
#define SRC_ID 12
#define SENDING_TIME 15


// Data
extern String data;

//members
extern bool status;
extern float scaleValue;
extern Communication *com;
extern logger *temperatureLog;

//functions
void sendMessage(int temperature);
void handleMesseage(uint32_t senderId , void *data);



#endif // __DATA_DEFINITIONS__