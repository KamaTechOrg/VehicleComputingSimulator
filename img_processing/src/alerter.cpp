#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include "../include/alerter.h"
#include "../include/alert.h"
#include "../include/distance.h"
using json = nlohmann::json;
using namespace std;
#define MIN_LEGAL_DISTANCE 1000
#define MIN_LEGAL_HEIGHT 3200

char *Alerter::makeAlertBuffer(int type, double distance)
{
    Alert alert(false, 1, type, distance);
    vector<char> serialized = alert.serialize();
    char *buffer = new char[serialized.size()];
    copy(serialized.begin(), serialized.end(), buffer);
    return buffer;
}

void Alerter::destroyAlertBuffer(char *buffer)
{
    delete[] buffer;
}

void Alerter::sendAlerts(const vector<DetectionObject> &output)
{
    for (DetectionObject detectionObject : output) {
        double distance = findDistanceToAlert(detectionObject);
        if (distance >= 0) {
            char *alertBuffer =
                makeAlertBuffer((int)detectionObject.type, distance);
            // TODO : use send comunication function.
            destroyAlertBuffer(alertBuffer);
        }
    }
}

double Alerter::findDistanceToAlert(const DetectionObject &detectionObject)
{
    int knowWidth;
    switch ((int)detectionObject.type) {
        case 0:
            if (detectionObject.position.y + detectionObject.position.height >
                MIN_LEGAL_HEIGHT)
                return 0;
            return -1;
        case 2:
            Distance distance;
            double distanceFromCar = distance.findDistance(detectionObject);
            if (distanceFromCar < MIN_LEGAL_DISTANCE)
                return distanceFromCar;
            return -1;
    }
    return -1;
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

    ofstream output_file("../../alert.json");
    if (!output_file) {
        cerr << "Error opening file for writing" << endl;
        return;
    }
    // Serialize JSON to the file
    output_file << j.dump(4);  // 4 is the indentation level
    output_file.close();

    cout << "JSON file created and written successfully." << endl;
}