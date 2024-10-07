#include <Arduino.h>
#include <iostream>

#define MIN_NORMAL_VALUE 2
#define REASONABLE_GAP 1
#define INVALID_CONTINUITY 3

using namespace std;


int currentIndex, lastCommaIndex, countLoop, countInvalid;
double average, previousValue;

void setup()
{
    Serial.begin(115200);
    Serial.println("Hello, ESP32!");
    currentIndex = 0;
    lastCommaIndex = -1;
    countLoop = 0;
    average = 0;
    previousValue = -1;
}

void loop()
{
    countLoop++;
    if (currentIndex < data.length()) {
        int nextCommaIndex = data.indexOf(',', currentIndex);

        if (nextCommaIndex == -1)
            nextCommaIndex = data.length();

        String numberStr = data.substring(currentIndex, nextCommaIndex);
        double currentValue =
            numberStr.toDouble();  // ממיר את המחרוזת למספר שלם

        Serial.println(currentValue);

        currentIndex = nextCommaIndex + 1;

        if (currentValue < MIN_NORMAL_VALUE) {
            countInvalid++;
            if (countInvalid > INVALID_CONTINUITY) {
                Serial.println("");
                Serial.println("send error:");
                Serial.print("---");
                Serial.print(currentValue);
                Serial.println("---");
                Serial.println("");
                countLoop = 0;
                average = 0;
            }
        }
        else {
            countInvalid = 0;
            if ((abs(currentValue - previousValue) < REASONABLE_GAP) ||
                previousValue == -1) {
                average += currentValue;
                previousValue = currentValue;
            }
            else {
                average += previousValue;
            }
        }
    }
    else {
        currentIndex = 0;
    }
    if (countLoop == 20) {
        Serial.println("");
        Serial.println("send:");
        Serial.print("---");
        Serial.print((average / 20));
        Serial.println("---");
        Serial.println("");

        countLoop = 0;
        average = 0;
    }
    delay(500);
}