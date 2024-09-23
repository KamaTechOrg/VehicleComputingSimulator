#include <iostream>

#include "communication.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

// Processing the data received from the complete message
void processData(uint32_t senderId,void *data)
{
  std::cout << "I'm userB: Received from id : " << senderId << ", data: " << static_cast<char *>(data)
            << std::endl;
  free(data);
}

int readIdFromJson() {
  // Read the json file
  ifstream f("config.json");

  // Check if the input is correct
  if (!f.is_open())
    cerr << "Failed to open config.json" << endl;

  json *data = NULL;

  // Try parse to json type
  try {
    data = new json(json::parse(f));
  } catch (exception e) {
    cout << e.what() << endl;
  } catch (...) {
    cout << "My Unknown Exception" << endl;
  }
  return (*data)["ID"];
}

int main() {
  // Preparing the parameters for the message
  uint32_t destID = 1;
  uint32_t srcID = readIdFromJson();
  cout << "srcID: " << srcID << endl;
  // Creating the communication object with the callback function to process the
  // data
  Communication comm(srcID, processData);
  // Starting communication with the server
  comm.startConnection();
  // Running in a time loop to receive messages and handle them
  while (true) {
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // Sending the message
    string field, value;
    cin >> field;
    cin >> value;
    field = to_string(srcID) + "|" + field + "|" + value;
    const char *message = field.c_str();
    size_t dataSize = strlen(message) + 1;

    comm.sendMessage((void *)message, dataSize, destID, srcID, false);
  }

  return 0;
}