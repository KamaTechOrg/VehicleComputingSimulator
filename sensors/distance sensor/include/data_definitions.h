#ifndef __DATA_DEFINITIONS__
#define __DATA_DEFINITIONS__

#include <Arduino.h>
#include <iostream>
#include <thread>
#include <chrono>

#include "../../../communication/src/communication.h"
#include "../../../logger/logger.h"

// Defines
#define MIN_NORMAL_VALUE 2
#define REASONABLE_GAP 1.5
#define INVALID_CONTINUITY 3
#define WAIT_TIME 400
#define SRC_ID 10
#define MSG_SIZE 280
#define SENDING_TIME 15

// Data
extern String data;

//members
extern float scaleValue;
extern Communication *com;
extern logger *distanceLog;

//functions
void sendMessage(std::string message, float distance);
void handleMesseage(uint32_t senderId , void *data);



#endif // __DATA_DEFINITIONS__