#include "alerter.h"
#include "alert.h"
#include "manager.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace std;

char *Alerter::makeAlertBuffer(const DetectionObject &detectionObject)
{
    Alert alert(false, 1, detectionObject.type, 0);
    vector<char> serialized = alert.serialize();
    char *buffer = new char[serialized.size()];
    copy(serialized.begin(), serialized.end(), buffer);
    return buffer;
}

void Alerter::sendAlerts(const vector<DetectionObject> &output)
{
    for (DetectionObject detectionObject : output) {
        // TODO : send to function that check if send alert...
        // if the function return true:
        char *alertBuffer = makeAlertBuffer(detectionObject);
        // Sending the message (buffer,size,dest,src,isbrodcast)
        // comm.sendMessage((void *)alertBuffer, sizeof(*alertBuffer), 1, 10,
        // false);
    }
}

void Alerter::makeFileJSON()
{
    cout << "in json";
    json j;

    // Serialize basic fields
    j["endianness"] = "little";  // Assuming little endian

    // Serialize fields
    j["fields"] = json::array();
    j["fields"].push_back(
        {{"size", 1}, {"type", "uint"}, {"name", "MessageType"}});
    j["fields"].push_back({{"size", 3}, {"type", "uint"}, {"name", "Level"}});
    j["fields"].push_back(
        {{"size", 4}, {"type", "uint"}, {"name", "ObjectType"}});
    j["fields"].push_back(
        {{"size", sizeof(double) * 8},  // Size of double in bits
         {"type", "float_fixed"},
         {"name", "ObjectDistance"}});
    j["fields"].push_back({{"size", sizeof(int) * 8},  // Size of int in bits
                           {"type", "uint"},
                           {"name", "CarSpeed"}});
    j["fields"].push_back({{"size", sizeof(int) * 8},  // Size of int in bits
                           {"type", "uint"},
                           {"name", "ObjectSpeed"}});

    ofstream output_file("../alert.json");
    if (!output_file) {
        cerr << "Error opening file for writing" << endl;
        return;
    }
    // Serialize JSON to the file
    output_file << j.dump(4);  // 4 is the indentation level
    output_file.close();
    Manager::imgLogger.logMessage(logger::LogLevel::INFO,
                                  "JSON file created and written successfully");
}
Alerter::Alerter() /*:comm(10,processData)*/
{
    // Starting communication with the server
    // comm.startConnection();
}
void processData(void *data)
{
    Manager::imgLogger.logMessage(
        logger::LogLevel::INFO,
        "Received data: " + string(static_cast<char *>(data)));
    free(data);
}