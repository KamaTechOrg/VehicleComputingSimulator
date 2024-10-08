#include "data_definitions.h"

using namespace std;

String data =
    "6.26,5.09,5.50,5.65,5.69,5.32,5.14,6.13,6.00,6.03,4.79,4.94,5.09,4.71,4."
    "35,4.55,5.09,5.20,5.71,5.53,5.43,5.43,5.38,5.42,5.22,5.65,5.42,5.57,5.40,"
    "4.38,5.75,5.34,4.45,5.12,4.90,5.10,15.16,5.12,5.17,5.30,5.04,5.04,14.58,4."
    "85,4.75,4.73,4.42,4.42,4.38,4.95,4.81,4.86,5.01,0.00,4.80,5.10,4.64,5.02,"
    "4.68,4.90,4.80,4.70,4.88,4.07,4.39,4.40,4.68,4.02,4.72,4.12,10.40,4.93,5."
    "22,5.36,5.54,5.76,5.32,5.12,5.17,5.05,0.00,5.27,10.85,5.48,5.66,5.43,6.82,"
    "5.13,5.38,5.58,5.65,6.09,5.52,5.67,5.51,5.74,5.48,5.63,5.48,5.85,5.66,25."
    "48,6.25,6.31,6.42,6.67,19.64,6.36,6.10,6.16,6.12,5.78,5.55,5.69,5.53,5.15,"
    "4.99,5.15,4.99,5.01,4.97,4.96,5.80,5.19,5.24,5.69,5.86,6.22,0.00,6.67,6."
    "65,6.05,26.03,16.55,6.59,0.08,0.08,0.08,0.08,0.08,0.08,0.31,0.08,0.08,0."
    "00,0.08,0.08,0.08,0.08,0.08,0.15,0.08,0.23,0.08,0.08,0.08,0.08,0.08,0.08,"
    "0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0."
    "08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,"
    "0.08,0.08,0.08,0.23,0.08,0.08,0.39,0.08,0.08,0.08,0.15,0.15,0.08,0.08,0."
    "08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,"
    "0.08,0.08,0.08,0.15,0.23,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0.08,0."
    "08,0.36,0.30,0.28,0.62,1.15,1.56,2.14,3.08,3.50,3.65,3.85,3.91,3.91,3.96,"
    "3.97,3.75,4.62,5.57,5.83,6.10,6.88,7.77,8.38,8.94,9.32,9.52,9.40,28.62,9."
    "62,9.75,9.88,9.90,10.05,10.22,10.29,10.36,10.36,10.28,10.18,10.32,10.19,9."
    "89,9.70,9.59,9.42,9.41,9.39,9.52,9.74,9.96,10.18,31.11,10.58,10.75,11.05,"
    "11.24,11.45,11.49,11.19,11.07,10.87,10.52,10.22,9.87,9.36,9.12,35.29,8.53,"
    "8.14,7.54,7.40,7.38,7.52,7.65,7.41,7.12,7.16,7.11,7.14,7.30,7.58,15.25,7."
    "72,15.92,32.83,8.41,8.39,8.45,8.45,8.62,17.62,9.05,9.27,9.62,9.88,10.33,"
    "10.48,10.61,11.00,11.31,11.55,11.74,11.62,11.68,11.81,11.94,0.00,12.74,12."
    "86,13.09,13.24,13.35,53.42,13.21,13.37,13.08,13.20,13.38,13.53,13.82,13."
    "85,13.74,13.22,13.17,13.02,12.88,12.52,12.27,12.06,11.81,11.56,11.35,11."
    "07,10.85,10.66,10.45,10.32,10.20,10.18,10.13,10.22,10.32,10.39,10.68,10."
    "69,10.65,55,14.77,15.14";


void sendMessage(string message, float distance)
{
    Serial.println("dis:");
    Serial.println(distance);

    std::vector<uint8_t> buffer;

    uint8_t bytes[sizeof(float)];
    std::memcpy(bytes, &distance, sizeof(float)); 
    buffer.insert(buffer.end(), bytes, bytes + sizeof(float));

    int i = 0;
    for (;i < message.size() &&  i < MSG_SIZE; buffer.push_back(static_cast<uint8_t>(message[i])), i++ )
        ;
    for (; i < MSG_SIZE; buffer.push_back(0), i++)
        ; 

    //distanceLog->logMessage(logger::LogLevel::INFO,"send message: "+ message +" to main control" );
    com->sendMessage(buffer.data(), buffer.size(), 1, SRC_ID, false);
}

void handleMesseage(uint32_t senderId , void *data)
{
    string  message = static_cast<char *>(data);
    //distanceLog->logMessage(logger::LogLevel::INFO,"i got message: "+ message +" from id: " + to_string(senderId));
    Serial.println("data recieved");
    Serial.println(message.c_str());

    if(message == "get closer")
        scaleValue *= 0.95;
    else if(message == "stay away")
        scaleValue *= 1.05;

    free(data);
}