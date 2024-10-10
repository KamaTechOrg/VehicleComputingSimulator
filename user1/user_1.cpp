#include <iostream>

#include "../communication/include/communication.h"
#include "../../hsm-client/include/crypto_api.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

// Processing the data received from the complete message
void processData(uint32_t senderId,void *data)
{
  std::cout << "I'm userA: Received from id : " << senderId << ", data: " << static_cast<char *>(data)
            << std::endl;
  free(data);
}

int readIdFromJson() {
  // Read the json file
  ifstream f("../config.json");

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

bool encryptData(const void *data, int dataLen, uint8_t *encryptedData,
                 size_t encryptedLength, uint32_t receiverId, uint32_t myId)
{
    CryptoClient client;

    // Encrypt the data
    CK_RV encryptResult = client.encrypt(
        myId,receiverId, data, dataLen, encryptedData, encryptedLength);
    // Check if encryption was successful
    if (encryptResult != CKR_OK)
        return false;
    return true;
}


int main() {
  // Preparing the parameters for the message
  uint32_t destID = 1;
  uint32_t srcID = readIdFromJson();
  cout << "srcID: " << srcID << endl;


  CryptoClient client;
  CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128, AESChainingMode::ECB, AsymmetricFunction::RSA);
  client.configure(srcID, config);
  client.configure(destID, config);
  client.bootSystem({{srcID, {KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT, KeyPermission::DECRYPT, KeyPermission::EXPORTABLE}},
                      {destID, {KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT, KeyPermission::DECRYPT, KeyPermission::EXPORTABLE}}});



  // Creating the communication object with the callback function to process the
  // data
  Communication comm(srcID, processData);
  // Starting communication with the server
  comm.startConnection();

  std::vector<uint8_t> buffer;
  // Temperature (32 bits, float, little-endian)
  float temperature = 25.0f;
  uint8_t tempBytes[4];
  std::memcpy(tempBytes, &temperature, sizeof(float));
  buffer.insert(buffer.end(), tempBytes, tempBytes + 4);

  // Pressure (32 bits, float, little-endian)
  float pressure = 20.0f;
  uint8_t pressBytes[4];
  std::memcpy(pressBytes, &pressure, sizeof(float));
  buffer.insert(buffer.end(), pressBytes, pressBytes + 4);

  // string msg = "nice aaa";
  // for (char c : msg) {
  //   buffer.push_back(static_cast<uint8_t>(c));
  // }


  //-------------
  // Get the length of the encrypted data

  size_t encryptedLength =
      client.getEncryptedLenClient(srcID, buffer.size());
  uint8_t encryptedData[encryptedLength];

  if (encryptData((const void *)buffer.data(), buffer.size(), encryptedData,
                  encryptedLength, destID, srcID))
    cout << "The message encrypted successfully" << endl;
  else
    cerr << "The message encryption failed" << endl;

  comm.sendMessage(encryptedData, encryptedLength, destID, srcID, false);
  //-------------


  // comm.sendMessage(buffer.data(), buffer.size(), destID, srcID, false);

///////////////////////////////////
  string s;
  cin >> s;

  buffer.clear();
  // Temperature (32 bits, float, little-endian)
  float temperature2 = 25.0f;
  uint8_t tempBytes2[4];
  std::memcpy(tempBytes2, &temperature2, sizeof(float));
  buffer.insert(buffer.end(), tempBytes2, tempBytes2 + 4);

  // Pressure (32 bits, float, little-endian)
  float pressure2 = 100.0f;
  uint8_t pressBytes2[4];
  std::memcpy(pressBytes2, &pressure2, sizeof(float));
  buffer.insert(buffer.end(), pressBytes2, pressBytes2 + 4);

  comm.sendMessage(buffer.data(), buffer.size(), destID, srcID, false);

////////////////////////////////////

  // Running in a time loop to receive messages and handle them
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return 0;
}