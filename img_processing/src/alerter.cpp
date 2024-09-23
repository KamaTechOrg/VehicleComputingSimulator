#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include "../include/alerter.h"
#include "../include/alert.h"
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
        comm.sendMessage((void *)alertBuffer, sizeof(*alertBuffer), 1, 4, false);
    }
}
void Alerter::makeFileJSON()
{
    json j;
    j["endianness"] = "little";
    j["fields"] = json::array();
    // field AlertDetails
    json alertDetailsJson;
    alertDetailsJson["name"] = "AlertDetails";
    alertDetailsJson["size"] = 8;
    alertDetailsJson["type"] = "bit_field";
    alertDetailsJson["fields"] = json::array(
        {{{"name", "MessageType"}, {"size", 1}, {"type", "unsigned_int"}},
         {{"name", "Level"}, {"size", 3}, {"type", "unsigned_int"}},
         {{"name", "ObjectType"}, {"size", 4}, {"type", "unsigned_int"}}});
    // add AlertDetails to JSON
    j["fields"].push_back(alertDetailsJson);
    // add ObjectDistance
    json objectDistanceJson;
    objectDistanceJson["name"] = "ObjectDistance";
    objectDistanceJson["size"] = 32;
    objectDistanceJson["type"] = "float_fixed";
    j["fields"].push_back(objectDistanceJson);
    // add CarSpeed
    json carSpeedJson;
    carSpeedJson["name"] = "CarSpeed";
    carSpeedJson["size"] = 32;
    carSpeedJson["type"] = "unsigned_int";
    j["fields"].push_back(carSpeedJson);
    // add ObjectSpeed
    json objectSpeedJson;
    objectSpeedJson["name"] = "ObjectSpeed";
    objectSpeedJson["size"] = 32;
    objectSpeedJson["type"] = "unsigned_int";
    j["fields"].push_back(objectSpeedJson);
    // Write the JSON to the file
    ofstream output_file("../../alert.json");
    if (output_file.is_open()) {
        output_file << j.dump(4); // 4 = Indent level for easier reading
        output_file.close();
    }
    else {
        std::cerr << "Error: Could not open file for writing" << std::endl;
    }
}
void processData(uint32_t senderId, void *data)
{
    std::cout << "Received data: " << static_cast<char *>(data) << std::endl;
    free(data);
}
Alerter::Alerter() : comm(4, processData) { // pass srcID and processData to the constructor
    // Starting communication with the server
    comm.startConnection();
}