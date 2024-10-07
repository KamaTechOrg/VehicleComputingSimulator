#include "data_definitions.h"

using namespace std;

int currentIndex, lastCommaIndex, countLoop, countInvalid;
int average, previousValue;
bool status;
float scaleValue;
Communication *com;
logger *speedLog;


void setup()
{
    Serial.begin(115200);
    currentIndex = countLoop = average = countInvalid = 0 ;
    lastCommaIndex = previousValue = -1;
    scaleValue = 1;
    status = true;
    com = new Communication(SRC_ID,handleMesseage);
    com->startConnection();
    speedLog = new logger("speed sensor");
    speedLog->logMessage(logger::LogLevel::INFO,"speed sensor");
    //Serial.println("speed sensor");


}

void loop()
{
    countLoop++;
    if (currentIndex < data.length()) {
        int nextCommaIndex = data.indexOf(',', currentIndex);

        if (nextCommaIndex == -1)
            nextCommaIndex = data.length();

        String numberStr = data.substring(currentIndex, nextCommaIndex);
        int currentValue = numberStr.toInt() * scaleValue;
        speedLog->logMessage(logger::LogLevel::DEBUG,"currentValue: " + to_string(currentValue));
         Serial.println(currentValue);

        currentIndex = nextCommaIndex + 1;

        if (currentValue > MAX_NORMAL_VALUE) {
            countInvalid++;
            speedLog->logMessage(logger::LogLevel::DEBUG, "invalid value " + to_string(currentValue) + " " + to_string(countInvalid) +" times" );

            if (countInvalid > INVALID_CONTINUITY) {
                status = false;
                Serial.println("\n send error:");
                Serial.println(("--- " + std::to_string(currentValue) + " --- \n").c_str());
                speedLog->logMessage(logger::LogLevel::ERROR, "send error:" +to_string(currentValue));
                countLoop = 0;
                average = 0;
                sendMessage("high speed",currentValue);
        
            }
        }
        else {
            countInvalid = 0;
            status = true;
            if ((abs(currentValue - previousValue) < REASONABLE_GAP) ||
                previousValue == -1) {
                average += currentValue;
                previousValue = currentValue;
                speedLog->logMessage(logger::LogLevel::INFO,"unlikely gap, previous value: " + to_string(previousValue) + " current value: " + to_string(currentValue));
            }
            else {
                average += previousValue;
            }
        }
    }
    else {
        currentIndex = 0;
    }
    if (countLoop == SENDING_TIME) {
        // Serial.println("\n send:");
        // Serial.println(("--- " + std::to_string((average / 20)) + " --- \n").c_str());
        speedLog->logMessage(logger::LogLevel::INFO,"send: " + to_string(average / 20));

        sendMessage("average current speed",(average/20));

        countLoop = 0;
        average = 0;

    }
    delay(WAIT_TIME);
}