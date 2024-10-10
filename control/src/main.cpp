#include <iostream>
#include <vector>
#include "sensor.h"
#include "input.h"
#include "full_condition.h"
#include "global_properties.h"
// #include "../parser_json/src/packet_parser.h"
#include "../../hsm-client/include/crypto_api.h"

using namespace std;

int main()
{
    CryptoClient client;
    vector<KeyPermission> permissions={KeyPermission::DECRYPT};
    CK_RV returnCode= client.addProccess(1, permissions);
    cout<<returnCode<<endl;
    GlobalProperties &instanceGP = GlobalProperties::getInstance();
    // Build the conditions from the bson file
    Input::s_buildConditions();

    GlobalProperties::controlLogger.logMessage(
        logger::LogLevel::INFO,
        "Initialized successfully, Starting Communication...");
    // Starting communication with the server
    instanceGP.comm->startConnection();

    // Running in a time loop to receive messages and handle them
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    GlobalProperties::controlLogger.cleanUp();

    return 0;
}