#include "data_definitions.h"

using namespace std;


String data =
    "89,93,95,96,99,102,104,106,105,423,107,108,112,115,116,118,120,121,121,"
    "119,121,118,119,121,367,125,125,126,127,127,127,127,127,127,128,124,120,"
    "119,118,116,113,111,109,107,105,103,100,98,96,94,93,92,92,91,92,93,94,190,"
    "98,99,99,99,9992,94,96,97,0,102,104,104,405,100,98,95,277,89,84,82,80,77,"
    "73,68,67,67,68,69,67,64,64,64,64,66"
    "56,46,49,51,51,48,46,55,54,54,43,44,46,42,39,41,46,47,51,50,49,49,48,49,"
    "47,51,49,50,48,39,52,48,40,46,44,46,45,46,46,48,45,45,44,43,43,169,158,40,"
    "39,44,43,44,45,45,43,137,42,45,42,44,43,168,88,36,39,39,42,36,42,37,47,"
    "177,47,48,50,52,48,46,46,45,39,47,49,49,51,49,61,46,48,50,51,55,49,51,49,"
    "52,49,51,49,53,51,57,56,57,173,60,59,57,55,55,55,52,50,51,50,46,45,46,45,"
    "45,45,44,52,93,47,51,53,56,58,60,60,54,117,49,59,0,0,0,0,0,0,0,0,0,0,0,0,"
    "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"
    "0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"
    "0,0,0,0,0,0,0,0,0,0,3,2,2,10,10,14,19,27,31,130,34,35,35,106,35,33,41,50,"
    "52,110,62,70,76,81,84,86,85,86,87,88,89,89,91,92,93,94,94,371,92,93,92,89,"
    "88,173,85,85,85,258,88,90,92,94,96,97,0,102,104,104,405,100,98,95,277,89,"
    "84,82,80,77,73,68,67,67,68,69,67,64,64,64,64,66,205,69,70,72,74,76,76,76,"
    "76,78,238,82,84,87,89,93,95,96,99,102,104,106,105,423,107,108,112,115,116,"
    "118,120,121,121,119,121,118,119,121,367,125,125,126,127,127,127,127,127,"
    "127,128,124,120,119,118,116,113,111,109,107,105,103,100,98,96,94,93,92,92,"
    "91,92,93,94,190,98,99,99,99,99";

void sendMessage(string message, int speed)
{
    std::vector<uint8_t> buffer;
    buffer.push_back(static_cast<uint8_t>(status ? 1 : 0));

    int i = 0;
    for (;i < message.size() &&  i < MSG_SIZE; buffer.push_back(static_cast<uint8_t>(message[i])), i++ )
        ;
    for (; i < MSG_SIZE; buffer.push_back(0), i++)
        ; 

    uint8_t intFieldBytes[4];
    std::memcpy(intFieldBytes, &speed, sizeof(int32_t)); 

    int32_t flags = speed;
    uint8_t flagsBytes[4];
    std::memcpy(flagsBytes, &flags, sizeof(int32_t));
    buffer.insert(buffer.end(), flagsBytes, flagsBytes + 4) ;

    speedLog->logMessage(logger::LogLevel::INFO,"send message: "+ message +" to main control" );
    com->sendMessage(buffer.data(), buffer.size(), 1, SRC_ID, false);
}

void handleMesseage(uint32_t senderId , void *data)
{
    string  message = static_cast<char *>(data);
    speedLog->logMessage(logger::LogLevel::INFO,"i got message: "+ message +" from id: " + to_string(senderId));
    // Serial.println("data recieved");
    // Serial.println(message.c_str());

    if(message == "slow down")
        scaleValue *= 0.95;
    else if(message == "speed up")
        scaleValue *= 1.05;

    free(data);
}
