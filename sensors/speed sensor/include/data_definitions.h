#ifndef __DATA_DEFINITIONS__
#define __DATA_DEFINITIONS__

#include <Arduino.h>
#include <iostream>
#include <thread>
#include <chrono>

#include "../../../communication/src/communication.h"
#include "../../../logger/logger.h"

// Defines
#define MAX_NORMAL_VALUE 125
#define REASONABLE_GAP 20
#define INVALID_CONTINUITY 6
#define WAIT_TIME 1000
#define SRC_ID 8
#define MSG_SIZE 280
#define SENDING_TIME 20




// Data
extern String data;

//members
extern bool status;
extern float scaleValue;
extern Communication *com;
extern logger *speedLog;

//functions
void sendMessage(std::string message, int speed);
void handleMesseage(uint32_t senderId , void *data);



#endif // __DATA_DEFINITIONS__