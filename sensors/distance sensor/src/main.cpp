#include "data_definitions.h"

using namespace std;

int currentIndex, lastCommaIndex, countLoop, countInvalid;
int previousValue;
float scaleValue, average;
Communication *com;
logger *distanceLog;


void setup()
{
    Serial.begin(115200);
    currentIndex = countLoop = average = countInvalid = 0 ;
    lastCommaIndex = previousValue = -1;
    scaleValue = 1;
    com = new Communication(SRC_ID,handleMesseage);
    com->startConnection();
    distanceLog = new logger("distance sensor");
    //distanceLog->logMessage(logger::LogLevel::INFO,"distance sensor");
    Serial.println("distance sensor");
}

void loop()
{
    countLoop++;
    if (currentIndex < data.length()) {
        int nextCommaIndex = data.indexOf(',', currentIndex);

        if (nextCommaIndex == -1)
            nextCommaIndex = data.length();

        String numberStr = data.substring(currentIndex, nextCommaIndex);
        float currentValue = numberStr.toFloat() * scaleValue;
        //distanceLog->logMessage(logger::LogLevel::DEBUG,"currentValue: " + to_string(currentValue));
        Serial.println(currentValue);

        currentIndex = nextCommaIndex + 1;

        if (currentValue < MIN_NORMAL_VALUE) {
            countInvalid++;
           // distanceLog->logMessage(logger::LogLevel::DEBUG, "invalid value " + to_string(currentValue) + " " + to_string(countInvalid) +" times" );

            if (countInvalid > INVALID_CONTINUITY) {
                Serial.println("\n send error:");
                Serial.println(("--- " + std::to_string(currentValue) + " --- \n").c_str());
                //distanceLog->logMessage(logger::LogLevel::ERROR, "send error:" +to_string(currentValue));
                countLoop = 0;
                average = 0;
                sendMessage("small distance",currentValue);
        
            }
        }
        else {
            countInvalid = 0;
            if ((abs(currentValue - previousValue) < REASONABLE_GAP) ||
                previousValue == -1) {
                average += currentValue;
                previousValue = currentValue;
                //distanceLog->logMessage(logger::LogLevel::INFO,"unlikely gap, previous value: " + to_string(previousValue) + " current value: " + to_string(currentValue));
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
        Serial.println("\n send:");
        Serial.println(("--- " + std::to_string((average / SENDING_TIME)) + " --- \n").c_str());
        //distanceLog->logMessage(logger::LogLevel::INFO,"send: " + to_string(average / SENDING_TIME));

        sendMessage("average current distance",(average/SENDING_TIME));

        countLoop = 0;
        average = 0;

    }
    delay(WAIT_TIME);
}