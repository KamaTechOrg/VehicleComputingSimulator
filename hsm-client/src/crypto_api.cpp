#include "../include/crypto_api.h"
#include "../include/general.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <grpcpp/client_context.h>
#include <grpcpp/support/status.h>
#include <iostream> 
#include <ostream>
#include <utility>
#include <iostream>
#include <vector>
#include <iostream>

/**
 * @brief Initiates the boot process for the system by sending user ID and their associated permissions to the server. 
 * @param usersIdspermissions A map where the key is a user ID (int) and the value is a vector of KeyPermission 
 *                            enums representing the permissions associated with that user ID.
 * @return CK_RV Returns CKR_OK if the boot process was successful, otherwise returns CKR_FUNCTION_FAILED.
 */
CK_RV CryptoClient::bootSystem(
    const std::map<int, std::vector<KeyPermission>> &usersIdspermissions)
{
    crypto::BootSystemRequest request;
    for (const auto &user : usersIdspermissions) {
        crypto::UserKeyPermissions* userKeyPermissions = request.add_usersidspermissions();
        userKeyPermissions->set_userid(user.first);

        for (const auto &permission : user.second) 
            userKeyPermissions->add_permissions(static_cast<crypto::KeyPermission>(permission)); 
    }
    crypto::Empty response;
    grpc::ClientContext context;
    grpc::Status status = stub_->bootSystem(&context, request , &response);
  
    if(status.ok())
       return CKR_OK;

    return CKR_FUNCTION_FAILED;
}


/**
 * @brief Adds a process for a given user with specified permissions by sending the data to the server.
 * @param userId The ID of the user for whom the process is being added.
 * @param permissions A vector of KeyPermission enums representing the permissions to be associated with the process.
 * @return CK_RV Returns CKR_OK if the process was added successfully, otherwise returns CKR_FUNCTION_FAILED.
 */
CK_RV CryptoClient::addProccess(
  int userId, std::vector<KeyPermission> &permissions)
{
  crypto::AddProcessRequest request;
  for (auto& permission : permissions)
        request.add_permissions(static_cast<crypto::KeyPermission>(permission));
  crypto::Empty response;
  grpc::ClientContext context;
  grpc::Status status = stub_->addProccess(&context, request , &response);
  
  if(status.ok())
      return CKR_OK;

  return CKR_FUNCTION_FAILED;
}


/**
 * @brief Configures cryptographic settings for a given user by sending the configuration data to the server.
 * @param userId The ID of the user for whom the configuration is being set.
 * @param config A CryptoConfig structure containing the cryptographic settings to be configured.
 * @return CK_RV Returns CKR_OK if the configuration was set successfully, otherwise returns CKR_FUNCTION_FAILED.
 */
CK_RV CryptoClient::configure(int userId, CryptoConfig config)
{
  crypto::ConfigureRequest request;
  request.set_userid(userId);
  auto* protoConfig = new crypto::CryptoConfig();
  protoConfig->set_hashfunction(static_cast<crypto::SHAAlgorithm>(config.hashFunction));
  protoConfig->set_aeskeylength(static_cast<crypto::AESKeyLength>(config.aesKeyLength));
  protoConfig->set_aeschainingmode(static_cast<crypto::AESChainingMode>(config.aesChainingMode));
  protoConfig->set_asymmetricfunction(static_cast<crypto::AsymmetricFunction>(config.asymmetricFunction));
  
  request.set_allocated_config(protoConfig);
  grpc::ClientContext context;
  crypto::Empty response;
  grpc::Status status = stub_->configure(&context, request, &response);
  if(status.ok())
    return CKR_OK;
  return CKR_FUNCTION_FAILED;
}

/**
* @brief Generates an AES key for a user with specified permissions.
* 
* @param userId The ID of the user requesting the key.
* @param aesKeyLength The desired length of the AES key.
* @param permissions The permissions associated with the key.
* @param destUserId The ID of the user who will receive the key.
* @return The generated AES key as a string, or an empty string on failure.
*/
std::string CryptoClient::generateAESKey(int userId, AESKeyLength aesKeyLength,
                           std::vector<KeyPermission> permissions,
                           int destUserId)
{
    crypto::GenerateAESKeyRequest request;
    request.set_destuserid(destUserId);
    request.set_keylength(static_cast<crypto::AESKeyLength>(aesKeyLength));
    request.set_user_id(userId);
    for (auto& permission : permissions)
        request.add_permissions(static_cast<crypto::KeyPermission>(permission));
    crypto::GenerateAESKeyResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->generateAESKey(&context, request, &response);
    if (status.ok()) 
        return response.aes_key();
    else {
        std::cerr << "RPC failed: " << status.error_message() << std::endl;
        return "";
    }
}

/**
* @brief Generates an RSA key pair for a user with specified permissions. 
* @param userId The ID of the user requesting the key pair.
* @param permissions The permissions associated with the key pair.
* @return A pair containing the public and private RSA keys as strings.
*/
std::pair<std::string, std::string> CryptoClient::generateRSAKeyPair(int userId,
                                std::vector<KeyPermission> permissions)
{
    crypto::GenerateKeyPairRequest request;
    request.set_user_id(userId);
    for (auto& perm : permissions)
        request.add_permissions(static_cast<crypto::KeyPermission>(perm));
    crypto::GenerateKeyPairResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->generateRSAKeyPair(&context, request, &response);
    if (status.ok()) 
        return std::make_pair(response.public_key(), response.private_key());
    else {
        std::cerr << "RPC failed: " << status.error_message() << std::endl;
        return {"", ""};;
    }
}

/**
* @brief Generates an ECC key pair for a user with specified permissions. 
* @param userId The ID of the user requesting the key pair.
* @param permissions The permissions associated with the key pair.
* @return A pair containing the private and public ECC keys as strings.
*/
std::pair<std::string, std::string> CryptoClient::generateECCKeyPair(int userId,
                                std::vector<KeyPermission> permissions)
{
    crypto::GenerateKeyPairRequest request;
    request.set_user_id(userId);
    for (auto& perm : permissions)
        request.add_permissions(static_cast<crypto::KeyPermission>(perm));
    crypto::GenerateKeyPairResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->generateECCKeyPair(&context, request, &response);
    if (status.ok()) 
        return std::make_pair(response.public_key(), response.private_key());
    else {
        std::cerr << "RPC failed: " << status.error_message() << std::endl;
        return {"", ""};;
    }
}

/**
* @brief Retrieves the length of the signature for the current hashing algorithm. 
* @return The length of the signature, or -1 on failure.
*/
size_t CryptoClient::getSignatureLength()
{
    crypto::GetHashLengthRequest request;
    crypto::GetLengthResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->getSignedDataLength(&context, request, &response);
    if (!status.ok())  {
        log(logger::LogLevel::ERROR,"RPC getSignatureLength failed");
        return -1;
    }

    return response.len();
}

/**
 * @brief Calculates the length of the signed data given the length of the input data.
 * @param inLen The length of the input data.
 * @return size_t The length of the data after signing, which is the input length plus the length of the signature.
 */
size_t CryptoClient::getSignedDataLength(size_t inLen)
{
  return inLen + getSignatureLength() + sizeof(SHAAlgorithm::SHA_256);
}

/**
 * @brief Calculates the length of the verified data given the length of the signed data.
 * @param inLen The length of the signed data.
 * @return size_t The length of the data after verification, which is the input length minus the length of the signature.
 */
size_t CryptoClient::getVerifiedDataLength(size_t inLen)
{
  return inLen - getSignatureLength() - sizeof(SHAAlgorithm::SHA_256);
}


/**
* @brief Signs the given input data using the specified hashing algorithm and key ID.
* @param senderId The ID of the sender.
* @param in Pointer to the input data to be signed.
* @param inLen Length of the input data.
* @param out Reference to a pointer that will hold the signed output data.
* @param outLen Reference to the length of the signed output data.
* @param hashFunc The hashing algorithm to use for signing.
* @param keyId The ID of the key to use for signing.
* @return CK_RV indicating the status of the signing operation.
*/
CK_RV CryptoClient::sign(int senderId, void *in, size_t inLen, uint8_t* &out,
                         size_t &outLen, SHAAlgorithm hashFunc,
                         std::string keyId) {
  std::vector<std::uint8_t> outData;
  std::vector<std::uint8_t *> chunks;
  grpc::Status status;
  crypto::SignResponse response;
  crypto::SignRequest request1;
  grpc::ClientContext context1;
  size_t i = 0, counter = (inLen + MAX_BLOCK-1)/MAX_BLOCK;
  if(counter == 1){
    request1.set_sender_id(senderId);
    request1.set_data(
        std::string(reinterpret_cast<const char *>(in), inLen % MAX_BLOCK));
    request1.set_counter(inLen / MAX_BLOCK + inLen % MAX_BLOCK > 0 ? 1 : 0);
    status = stub_->signUpdate(&context1, request1, &response);
    if (!status.ok()) {
      log(logger::LogLevel::ERROR,"RPC sign failed");
      for (auto ptr : chunks)
        delete[] ptr;

      return CKR_FUNCTION_FAILED;
    }
  }
  for (; i < inLen / MAX_BLOCK; i++) {
    chunks.push_back(new std::uint8_t[MAX_BLOCK]);
    memcpy(chunks[i], (unsigned char *)in + i * MAX_BLOCK, MAX_BLOCK);
    outData.insert(outData.end(), chunks[i], chunks[i] + MAX_BLOCK);
  }
  if(inLen % MAX_BLOCK){
  chunks.push_back(new std::uint8_t[inLen % MAX_BLOCK]);
  memcpy(chunks[i], (unsigned char *)in + i * MAX_BLOCK, inLen % MAX_BLOCK);
  outData.insert(outData.end(), chunks[i], chunks[i] + inLen % MAX_BLOCK);
  }
  
  for (size_t i = 0; i < chunks.size(); ++i) {
    crypto::SignRequest request;
    grpc::ClientContext context;
    request.set_sender_id(senderId);
    request.set_data(
        std::string(reinterpret_cast<const char *>(chunks[i]), MAX_BLOCK));
    request.set_counter(inLen / MAX_BLOCK + inLen % MAX_BLOCK > 0 ? 1 : 0);
    if (i == chunks.size() - 1){
      request.set_key_id(keyId);
      status = stub_->signFinalize(&context, request, &response);
    }
    else
      status = stub_->signUpdate(&context, request, &response);
    if (!status.ok()) {
      log(logger::LogLevel::ERROR,"RPC sign failed");
      for (auto ptr : chunks)
        delete[] ptr;

      return CKR_FUNCTION_FAILED;
    }
 }
  std::vector<unsigned char> signatureunsigned(response.signature().begin(), response.signature().end());
  uint8_t* metadataPtr = (uint8_t*)out;
  memcpy(metadataPtr, &hashFunc, sizeof(hashFunc));
  metadataPtr += sizeof(hashFunc);
  memcpy((uint8_t *)metadataPtr, response.signature().data(),response.signature().size());
  memcpy((uint8_t*)metadataPtr + response.signature().size(), outData.data(), outData.size());

  for (auto ptr : chunks)
    delete[] ptr;

  return CKR_OK;
}

/**
 * @brief Verifies the signature of the provided data.
 * @param senderId The ID of the sender.
 * @param recieverId The ID of the receiver.
 * @param in Pointer to the signed data.
 * @param inLen Length of the signed data.
 * @param out Pointer to the buffer for the verification output.
 * @param outLen Reference to the length of the output data.
 * @param hashFunc The hash algorithm used for verification.
 * @param keyId The ID of the key used for verification.
 * @return CK_RV Status code indicating the success or failure of the operation.
 */
CK_RV CryptoClient::verify(int senderId, int recieverId, void *in, size_t inLen,
                           void *out, size_t &outLen,
                           std::string keyId) {
                            crypto::VerifyResponse response;
     
    uint8_t* metadataPtr = (uint8_t*)in;
    SHAAlgorithm hashFunc;
    memcpy(&hashFunc, metadataPtr, sizeof(hashFunc));
    metadataPtr += sizeof(hashFunc);
    grpc::ClientContext context1;
    grpc::Status status;
    crypto::VerifyRequest request1;
    size_t signatureLength = getSignatureLength();
    uint8_t *signature = new uint8_t[signatureLength];
    memcpy(signature, metadataPtr, signatureLength);
    inLen -= signatureLength + sizeof(hashFunc);
    metadataPtr += signatureLength;
    size_t counter = (inLen + MAX_BLOCK -1)/MAX_BLOCK;
    std::vector<std::uint8_t *> chunks;
    if(counter == 1){
        request1.set_sender_id(senderId);
        request1.set_data(
        std::string(reinterpret_cast<const char *>(metadataPtr), inLen % MAX_BLOCK));
        request1.set_receiver_id(recieverId);
        request1.set_counter(counter);
        request1.set_hash_func(static_cast<crypto::SHAAlgorithm>(hashFunc));
        request1.set_signature(signature, signatureLength);
        status = stub_->verifyUpdate(&context1, request1, &response);
        if (!status.ok()) {
          log(logger::LogLevel::ERROR,"RPC verify failed");
          for (auto ptr : chunks)
            delete[] ptr;
          
          return CKR_FUNCTION_FAILED;
        }
    }
    size_t i = 0;
    for (; i < inLen / MAX_BLOCK; i++) {
        chunks.push_back(new std::uint8_t[MAX_BLOCK]);
        memcpy(chunks[i], metadataPtr  + i * MAX_BLOCK,
           MAX_BLOCK);
        
    }
    chunks.push_back(new std::uint8_t[inLen % MAX_BLOCK]);
    memcpy(chunks[i], metadataPtr + i * MAX_BLOCK,inLen % MAX_BLOCK);

    for (i = 0; i < chunks.size(); ++i) {
        crypto::VerifyRequest request;
        request.set_sender_id(senderId);
        request.set_receiver_id(recieverId);
        request.set_hash_func(static_cast<crypto::SHAAlgorithm>(hashFunc));
        request.set_key_id(keyId);
        request.set_data(
        std::string(reinterpret_cast<const char *>(chunks[i]), MAX_BLOCK));
        if(i == chunks.size() - 1){
            request.set_signature(signature, signatureLength);
        }
        grpc::ClientContext context;
        if (i == chunks.size() - 1)
           status = stub_->verifyFinalize(&context, request, &response);
        else
           status = stub_->verifyUpdate(&context, request, &response);
    
        if (!status.ok()) {
           log(logger::LogLevel::ERROR,"RPC verify failed");
           for (auto ptr : chunks)
              delete[] ptr;
        
           return CKR_SIGNATURE_INVALID;
        }
    }
    memcpy(out, metadataPtr, outLen);
    
    for (auto ptr : chunks)
       delete[] ptr;
    
    return CKR_OK;
}

/**
  @brief Retrieves the public ECC key for a specified user by user ID.
  @param userId The ID of the user.
  @return The public ECC key as a string
*/
std::string CryptoClient::getPublicECCKeyByUserId(int userId)
{
    crypto::KeyRequest request;
    request.set_user_id(userId);
    crypto::KeyResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->getPublicECCKeyByUserId(&context, request, &response);
    
    if(!status.ok()){
      log(logger::LogLevel::ERROR,"RPC getPublicECCKeyByUserId failed");
      
      return "";
    }

    return response.key();
}

/** 
 @brief Retrieves the public RSA key for a specified user by user ID.
 @param userId The ID of the user.
 @return The public RSA key as a string. If the RPC call fails, returns an empty string.
*/
std::string CryptoClient::getPublicRSAKeyByUserId(int userId)
{
    crypto::KeyRequest request;
    request.set_user_id(userId);
    crypto::KeyResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->getPublicRSAKeyByUserId(&context, request, &response);
    if(!status.ok()){
        log(logger::LogLevel::ERROR,"RPC getPublicRSAKeyByUserId failed");
        
        return "";
    }
   
    return response.key();
}

/**
 @brief Gets the length of the data when encrypted using ECC.
 @param dataLen The length of the input data.
 @return The length of the encrypted data. If the RPC call fails, returns -1.
*/
size_t CryptoClient::getECCencryptedLength()
{
    crypto::GetLengthRequest request;
    crypto::GetLengthResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->getECCencryptedLength(&context, request, &response);
     if(!status.ok()){
        log(logger::LogLevel::ERROR,"RPC getECCencryptedLength failed");
        
        return -1;
    }

    return response.len();
}

/**
 @brief Gets the length of the data when decrypted using ECC.
 @param dataLen The length of the input data.
 @return The length of the encrypted data. If the RPC call fails, returns -1.
*/
size_t CryptoClient::getECCdecryptedLength()
{
    crypto::GetLengthRequest request;
    crypto::GetLengthResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->getECCDecryptedLength(&context, request, &response);
     if(!status.ok()){
        log(logger::LogLevel::ERROR,"RPC getECCdecryptedLength failed");
        
        return -1;
    }

    return response.len();
}

/**
 @brief Encrypts data using ECC.
 @param senderId The ID of the sender.
 @param keyId The ID of the key to be used for encryption.
 @param in Pointer to the input data to be encrypted.
 @param inLen Length of the input data.
 @param out Pointer to the output buffer where encrypted data will be stored.
 @param outLen Reference to the variable that will hold the length of the output data.
 @return CKR_OK on success, or CKR_FUNCTION_FAILED if the RPC call fails.
*/
CK_RV CryptoClient::ECCencrypt(int senderId, std::string keyId, void *in, size_t inLen,
                 void *out, size_t &outLen)
{
    crypto::AsymetricEncryptRequest request;
    request.set_senderid(senderId);
    request.set_keyid(keyId);
    request.set_data(in,inLen);
    crypto::AsymetricEncryptResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->ECCencrypt(&context, request, &response);
    if(!status.ok()){
        log(logger::LogLevel::ERROR,"RPC ECCencrypt failed");
        
        return CKR_FUNCTION_FAILED;
    }
    memcpy(out, response.encrypted_data().data(), outLen);
    
    return CKR_OK;
}

/**
 @brief  Decrypts data using ECC.
 @param receiverId The ID of the receiver.
 @param keyId The ID of the key to be used for decryption.
 @param in Pointer to the input data to be decrypted.
 @param inLen Length of the input data.
 @param out Pointer to the output buffer where decrypted data will be stored.
 @param outLen Reference to the variable that will hold the length of the output data.
 @return CKR_OK on success, or CKR_FUNCTION_FAILED if the RPC call fails.
*/
CK_RV CryptoClient::ECCdecrypt(int receiverId, std::string keyId, void *in, size_t inLen,
                 void *out, size_t &outLen)
{
    crypto::AsymetricDecryptRequest request;
    request.set_receiverid(receiverId);
    request.set_keyid(keyId);
    request.set_data(in, inLen);
    crypto::AsymetricDecryptResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->ECCdecrypt(&context, request, &response);
    if(!status.ok()){
        log(logger::LogLevel::ERROR,"RPC ECCdecrypt failed");
        
        return CKR_FUNCTION_FAILED;
    }
    memcpy(out, response.decrypted_data().data(), outLen);
    
    return CKR_OK;
}
/**
 Gets the length of the data when encrypted using RSA.
 @param dataLen The length of the input data.
 @return The length of the encrypted data. If the RPC call fails, returns -1.
*/
size_t CryptoClient::getRSAencryptedLength()
{
    crypto::GetLengthRequest request;
    crypto::GetLengthResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->getRSAencryptedLength(&context, request, &response);
     if(!status.ok()){
        log(logger::LogLevel::ERROR,"RPC getSignatureLength failed");
        
        return -1;
    }

    return response.len();
};
/**
 Gets the length of the data when decrypted using RSA.
 @param dataLen The length of the input data.
 @return The length of the encrypted data. If the RPC call fails, returns -1.
*/
size_t CryptoClient::getRSAdecryptedLength()
{
    crypto::GetLengthRequest request;
    crypto::GetLengthResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->getRSAdecryptedLength(&context, request, &response);
     if(!status.ok()){
        log(logger::LogLevel::ERROR,"RPC getSignatureLength failed");
        
        return -1;
    }

    return response.len();
};

/** 
 @brief Encrypts data using RSA.
 @param userId The ID of the user sending the data.
 @param keyId The ID of the key to be used for encryption.
 @param in Pointer to the input data to be encrypted.
 @param inLen Length of the input data.
 @param out Pointer to the output buffer where encrypted data will be stored.
 @param outLen Length of the output buffer.
 @return CKR_OK on success, or CKR_FUNCTION_FAILED if the RPC call fails.
*/
CK_RV CryptoClient::RSAencrypt(int userId, std::string keyId, void *in, size_t inLen,
                 void *out, size_t outLen)
                                        {
    crypto::AsymetricEncryptRequest request;
    request.set_senderid(userId);
    request.set_keyid(keyId);
    request.set_data(in, inLen);
    crypto::AsymetricEncryptResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->RSAencrypt(&context, request, &response);
    if(!status.ok()){
        log(logger::LogLevel::ERROR,"RPC RSAencrypt failed");
        return CKR_FUNCTION_FAILED;
    }
    memcpy(out, response.encrypted_data().data(), outLen);
    
    return CKR_OK;
}

/** 
 @brief  Decrypts data using RSA.
 @param userId The ID of the user receiving the data.
 @param keyId The ID of the key to be used for decryption.
 @param in Pointer to the input data to be decrypted.
 @param inLen Length of the input data.
 @param out Pointer to the output buffer where decrypted data will be stored.
 @param outLen Pointer to a variable that will hold the length of the output data.
 @return CKR_OK on success, or CKR_FUNCTION_FAILED if the RPC call fails.
*/
CK_RV CryptoClient::RSAdecrypt(int userId, std::string keyId, void *in, size_t inLen,
                 void *out, size_t &outLen)
{
    crypto::AsymetricDecryptRequest request;
    request.set_receiverid(userId);
    request.set_keyid(keyId);
    request.set_data(in, inLen);
    crypto::AsymetricDecryptResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->RSAdecrypt(&context, request, &response);
    if(!status.ok()){
        log(logger::LogLevel::ERROR,"RPC RSAdecrypt failed");
        return CKR_FUNCTION_FAILED;
    }
    memcpy(out, response.decrypted_data().data(), outLen);
    
    return CKR_OK;
}


/**
 Gets the length of the data chunk when encrypted using AES.
 @param dataLen The length of the chunk.
 @param isFirst If the chunk is first on the streaming.
 @param chainingMode aes chaining mode.
 @return The length of the encrypted data. If the RPC call fails, returns -1.
*/
size_t CryptoClient::getAESencryptedLength(size_t dataLen, bool isFirst, AESChainingMode chainingMode)
{
    crypto::GetAESLengthRequest request;
    request.set_datalen(dataLen);
    request.set_isfirst(isFirst);
    request.set_chainingmode(static_cast<crypto::AESChainingMode>(chainingMode));
    crypto::GetLengthResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->getAESencryptedLength(&context, request, &response);
     if(!status.ok()){
        log(logger::LogLevel::ERROR,"RPC getAESencryptedLength failed");
       
       return -1;
    }

    return response.len();

}

/**
 Gets the length of the data chunk when decrypted using AES.
 @param dataLen The length of the chunk.
 @param isFirst If the chunk is first on the streaming.
 @param chainingMode aes chaining mode.
 @return The length of the decrypted data. If the RPC call fails, returns -1.
*/
size_t CryptoClient::getAESdecryptedLength(size_t dataLen, bool isFirst, AESChainingMode chainingMode)
{
    crypto::GetAESLengthRequest request;
    request.set_datalen(dataLen);
    request.set_isfirst(isFirst);
    request.set_chainingmode(static_cast<crypto::AESChainingMode>(chainingMode));
    crypto::GetLengthResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->getAESdecryptedLength(&context, request, &response);
     if(!status.ok()){
        log(logger::LogLevel::ERROR,"RPC getAESdecryptedLength failed");
      
        return -1;
    }

    return response.len();
}

/**
 Gets the length of the data input when decrypted using AES.
 @param dataLen The length of the chunk.
 @param chainingMode aes chaining mode.
 @return The length of the decrypted data. If the RPC call fails, returns -1.
*/
size_t CryptoClient::getAESencryptedLengthClient(size_t dataLen, AESChainingMode chainingMode, AESKeyLength keyLength, AsymmetricFunction func, const std::string &keyId)
{
    size_t length = sizeof(keyLength) + sizeof(chainingMode) + sizeof(func) + keyId.length() + 1;
    for (int i = 0; i < dataLen / MAX_BLOCK; i++)
        length += getAESencryptedLength(MAX_BLOCK, i == 0, chainingMode);
    if (dataLen % MAX_BLOCK)
        length += getAESencryptedLength(dataLen % MAX_BLOCK, dataLen < MAX_BLOCK, chainingMode);

    return length;
}

size_t CryptoClient::getAESdecryptedLengthClient(void* in ,size_t dataLen)
{
    uint8_t* metadataPtr = (uint8_t*)in;
    AESKeyLength keyLength;
    memcpy(&keyLength, metadataPtr, sizeof(keyLength));
    metadataPtr += sizeof(keyLength);

    AESChainingMode chainingMode;
    memcpy(&chainingMode, metadataPtr, sizeof(chainingMode));
    metadataPtr += sizeof(chainingMode);

    AsymmetricFunction func;
    memcpy(&func, metadataPtr, sizeof(func));
    metadataPtr += sizeof(func);

    std::string keyId((char*)metadataPtr);
    metadataPtr += keyId.length() + 1;

    size_t encryptedDataLen = dataLen - (metadataPtr - (uint8_t*)in);
    uint8_t* encryptedData = metadataPtr;

    size_t encryptBlock = getAESencryptedLength(MAX_BLOCK, false, chainingMode);
    size_t firstBlock = getAESencryptedLength(MAX_BLOCK, true, chainingMode);

    size_t length = 0;
    length += dataLen < firstBlock ? getAESdecryptedLength(dataLen, true, chainingMode) : getDecryptedLen(firstBlock, true, chainingMode);
    if (dataLen > firstBlock) {
        for (int i = 0; i < (dataLen - firstBlock) / encryptBlock; i++)
            length += getAESdecryptedLength(encryptBlock, i == 0, chainingMode);
    }
    if ((dataLen - firstBlock) % encryptBlock && dataLen > firstBlock)
        length += getAESdecryptedLength((dataLen - firstBlock) % encryptBlock, false, chainingMode);

    return length;
}

/**
 @brief Encrypts data using AES.
 @param senderId The ID of the sender.
 @param recieverId The ID of the receiver.
 @param in Pointer to the input data to be encrypted.
 @param inLen Length of the input data.
 @param out Reference to the pointer where the encrypted data will be stored.
 @param outLen Reference to the variable that will hold the length of the output data.
 @param func The asymmetric function used for encryption.
 @param keyLength The AES key length.
 @param chainingMode The AES chaining mode used for encryption.
 @param keyId The ID of the key to be used for encryption.
 @return CKR_OK on success, or CKR_FUNCTION_FAILED if the RPC call fails.
*/
CK_RV CryptoClient::AESencrypt(int senderId, int receiverId, void *in, size_t inLen,
                 void *&out, unsigned int &outLen, AsymmetricFunction func,
                 AESKeyLength keyLength, AESChainingMode chainingMode,
                 std::string keyId)
{
    std::vector<std::uint8_t*> chunks;
    size_t i = 0;
    for (; i < inLen / MAX_BLOCK; i++) {
        chunks.push_back(new std::uint8_t[MAX_BLOCK]);
        memcpy(chunks[i], (unsigned char*)in + i * MAX_BLOCK, MAX_BLOCK);
    }

    chunks.push_back(new std::uint8_t[inLen % MAX_BLOCK]);
    memcpy(chunks[i], (unsigned char*)in + i * MAX_BLOCK, inLen % MAX_BLOCK);

    std::vector<std::uint8_t> outData;
    int count = 0;
    for (const auto& chunk : chunks) {
        count++;
        crypto::AESEncryptRequest request;
        request.set_isfirst(count == 1);
        request.set_key_length(static_cast<crypto::AESKeyLength>(keyLength));
        request.set_sender_id(senderId);
        request.set_key_id(keyId);
        request.set_receiver_id(receiverId);
        request.set_data(std::string(reinterpret_cast<const char*>(chunk), (count == chunks.size() && inLen % MAX_BLOCK != 0) ? inLen % MAX_BLOCK : MAX_BLOCK));
        request.set_counter((inLen + MAX_BLOCK - 1) / MAX_BLOCK);
        request.set_func(static_cast<crypto::AsymmetricFunction>(func));
        request.set_chainingmode(static_cast<crypto::AESChainingMode>(chainingMode));

        crypto::AESEncryptResponse response;
        grpc::ClientContext context;

        grpc::Status status = stub_->AESencrypt(&context, request, &response);

        if (!status.ok()) {
            log(logger::LogLevel::ERROR, "RPC AESencrypt failed");
            for (auto ptr : chunks)
                delete[] ptr;

            return CKR_FUNCTION_FAILED;
        }

        outData.insert(outData.end(), response.encrypted_data().data(), response.encrypted_data().data() + response.encrypted_data().length());
    }

    // Add the metadata to the output
    size_t metadataSize = sizeof(keyLength) + sizeof(chainingMode) + sizeof(func) + keyId.length() + 1;
    outLen = outData.size() + metadataSize;
    out = new uint8_t[outLen];

    // Copy the metadata
    uint8_t* metadataPtr = (uint8_t*)out;
    memcpy(metadataPtr, &keyLength, sizeof(keyLength));
    metadataPtr += sizeof(keyLength);
    memcpy(metadataPtr, &chainingMode, sizeof(chainingMode));
    metadataPtr += sizeof(chainingMode);
    memcpy(metadataPtr, &func, sizeof(func));
    metadataPtr += sizeof(func);
    strcpy((char*)metadataPtr, keyId.c_str());
    metadataPtr += keyId.length() + 1;

    // Copy the encrypted data
    memcpy(metadataPtr, outData.data(), outData.size());

    for (auto ptr : chunks)
        delete[] ptr;

    return CKR_OK;
}

/**
 * @brief Decrypts the input data using AES encryption.
 * @param senderId The ID of the sender.
 * @param recieverId The ID of the receiver.
 * @param in Pointer to the input encrypted data.
 * @param inLen Length of the input data.
 * @param out Reference to a pointer where the decrypted output will be stored.
 * @param outLen Reference to the length of the decrypted output.
 * @return CK_RV Status of the operation (CKR_OK on success).
*/
CK_RV CryptoClient::AESdecrypt(int senderId, int receiverId, void *in, size_t inLen, void *&out, size_t &outLen)
{
    // Extract the metadata
    uint8_t* metadataPtr = (uint8_t*)in;
    AESKeyLength keyLength;
    memcpy(&keyLength, metadataPtr, sizeof(keyLength));
    metadataPtr += sizeof(keyLength);

    AESChainingMode chainingMode;
    memcpy(&chainingMode, metadataPtr, sizeof(chainingMode));
    metadataPtr += sizeof(chainingMode);

    AsymmetricFunction func;
    memcpy(&func, metadataPtr, sizeof(func));
    metadataPtr += sizeof(func);

    std::string keyId((char*)metadataPtr);
    metadataPtr += keyId.length() + 1;

    size_t encryptedDataLen = inLen - (metadataPtr - (uint8_t*)in);
    uint8_t* encryptedData = metadataPtr;

    size_t encryptBlock = getAESencryptedLength(MAX_BLOCK, false, chainingMode);
    size_t firstBlock = getAESencryptedLength(MAX_BLOCK, true, chainingMode);
    size_t counter = encryptedDataLen > firstBlock ? (1 + (encryptedDataLen - firstBlock) / encryptBlock + ((encryptedDataLen - firstBlock) % encryptBlock ? 1 : 0)) : 1;
    std::vector<std::uint8_t*> chunks;
    size_t i = 0;
    if (encryptedDataLen > firstBlock) {
        chunks.push_back(new std::uint8_t[firstBlock]);
        memcpy(chunks[i], encryptedData, firstBlock);
        i++;
        for (; i < 1 + (encryptedDataLen - firstBlock) / encryptBlock; i++) {
            chunks.push_back(new std::uint8_t[encryptBlock]);
            memcpy(chunks[i], encryptedData + ((i - 1) * encryptBlock) + firstBlock, encryptBlock);
        }
        if ((encryptedDataLen - firstBlock) % encryptBlock != 0) {
            chunks.push_back(new std::uint8_t[((encryptedDataLen - firstBlock) % encryptBlock)]);
            memcpy(chunks[i], encryptedData + (i > 0 ? (((i - 1) * encryptBlock) + firstBlock) : 0), ((encryptedDataLen - firstBlock) % encryptBlock));
        }
    } else {
        chunks.push_back(new std::uint8_t[encryptedDataLen]);
        memcpy(chunks[i], encryptedData, encryptedDataLen);
    }
    std::vector<std::uint8_t> outData;
    bool isFirst = true;
    size_t count = 0;
    for (const auto &chunk : chunks) {
        count++;
        crypto::AESDecryptRequest request;
        request.set_key_id(keyId);
        request.set_sender_id(senderId);
        request.set_chainingmode(static_cast<crypto::AESChainingMode>(chainingMode));
        request.set_func(static_cast<crypto::AsymmetricFunction>(func));
        request.set_key_length(static_cast<crypto::AESKeyLength>(keyLength));
        request.set_receiver_id(receiverId);
        request.set_isfirst(isFirst);
        if (isFirst)
            request.set_data_in(std::string(reinterpret_cast<const char *>(chunk), encryptedDataLen > firstBlock ? firstBlock : encryptedDataLen));
        else
            request.set_data_in(std::string(reinterpret_cast<const char *>(chunk), (count == chunks.size() && (encryptedDataLen - firstBlock) % encryptBlock) ? (encryptedDataLen - firstBlock) % encryptBlock : encryptBlock));

        isFirst = false;
        request.set_counter(chunks.size());
        crypto::AESDecryptResponse response;
        grpc::ClientContext context;
        grpc::Status status = stub_->AESdecrypt(&context, request, &response);
        if (!status.ok()) {
            log(logger::LogLevel::ERROR, "RPC decrypt failed");
            for (auto ptr : chunks)
                delete[] ptr;

            return CKR_FUNCTION_FAILED;
        }
        outData.insert(outData.end(), response.decrypted_data().data(), response.decrypted_data().data() + response.decrypted_data().length());
    }
    outLen = outData.size();
    out = new uint8_t[outLen];
    memcpy(out, outData.data(), outLen);
    for (auto ptr : chunks)
        delete[] ptr;

    return CKR_OK;
}

/**
 @brief Gets the length of the chunk when encrypted data.
 @param senderId The id of data sender. 
 @param isFirst If the chunk is first on the streaming.
 @param dataLen The length of the chunk.
 @return The length of the encrypted data. If the RPC call fails, returns -1.
*/
size_t CryptoClient::getEncryptedLen(int senderId, size_t inLen, bool isfirst)
{
    crypto::GetWholeLength request;
    request.set_senderid(senderId);
    request.set_inlen(inLen);
    request.set_isfirst(isfirst);
    crypto::GetLengthResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->getEncryptedLen(&context, request, &response);
     if(!status.ok()){
      log(logger::LogLevel::ERROR,"RPC getEncryptedLen failed");
      return -1;
    }
    return response.len() ;
}

/**
 @brief Gets the length of the chunk when encrypted data.
 @param senderId The id of data sender. 
 @param isFirst If the chunk is first on the streaming.
 @param dataLen The length of the chunk.
 @return The length of the decrypted data. If the RPC call fails, returns -1.
*/
size_t CryptoClient::getDecryptedLen(int senderId, size_t encryptedLength, bool isfirst)
{
    crypto::GetWholeLength request;
    request.set_senderid(senderId);
    request.set_inlen(encryptedLength - getSignatureLength());
    request.set_isfirst(isfirst);
    crypto::GetLengthResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->getDecryptedLen(&context, request, &response);
     if(!status.ok()){
        log(logger::LogLevel::ERROR,"RPC getDecryptedLen failed");
      return -1;
    }

    return response.len();
}

/**
 @brief Gets the length of the input data when encrypted data.
 @param senderId The id of data sender. 
 @param dataLen The length of the input data.
 @return The length of the encrypted data. If the RPC call fails, returns -1.
*/
size_t CryptoClient::getEncryptedLenClient(int senderId, size_t inLen)
{
    size_t length = 0;
    for(int i = 0; i < inLen / MAX_BLOCK; i++)
      length += getEncryptedLen(senderId, MAX_BLOCK, i == 0);
    
    if(inLen % MAX_BLOCK)
      length += getEncryptedLen(senderId, inLen % MAX_BLOCK, inLen < MAX_BLOCK);
    
    return length + getSignatureLength();
}

/**
 @brief Gets the length of the chunk when decrypted data.
 @param senderId The id of data sender. 
 @param dataLen The length of the chunk.
 @return The length of the decrypted data. If the RPC call fails, returns -1.
*/
size_t CryptoClient::getDecryptedLenClient(int senderId, size_t encryptedLength)
{
    size_t encryptBlock = getEncryptedLen(senderId, MAX_BLOCK, false);
    size_t firstBlock = getEncryptedLen(senderId, MAX_BLOCK, true);

   size_t length = 0;
   length += encryptedLength < firstBlock? getDecryptedLen(senderId, encryptedLength,  true): getDecryptedLen( senderId,  firstBlock, true);
    if(encryptedLength > firstBlock){
        for(int i = 0; i < (encryptedLength - firstBlock) / encryptBlock; i++)
            length += getDecryptedLen(senderId, encryptBlock, i == 0);  
    }
    if((encryptedLength - firstBlock) % encryptBlock && encryptedLength > firstBlock)
        length += getEncryptedLen(senderId, (encryptedLength - firstBlock) % encryptBlock,  false);
    
    return length;
}

/**
 * @brief Encrypts the input data.
 * @param senderId The ID of the sender.
 * @param receiverId The ID of the receiver.
 * @param in Pointer to the input data.
 * @param inLen Length of the input data.
 * @param out Pointer where the encrypted output will be stored.
 * @param outLen Reference to the length of the encrypted output.
 * @return CK_RV Status of the operation (CKR_OK on success).
 */
CK_RV CryptoClient::encrypt(int senderId, int receiverId, void *in, size_t inLen, void *out,
            size_t &outLen)
{
    crypto::EncryptResponse response;
    size_t counter = (inLen + MAX_BLOCK - 1) / MAX_BLOCK;
    std::vector<std::uint8_t*> chunks;
    size_t i = 0;
    for(; i < inLen / MAX_BLOCK ; i++){
        chunks.push_back(new std::uint8_t[MAX_BLOCK]);
        memcpy(chunks[i], (unsigned char*)in + i * MAX_BLOCK, MAX_BLOCK);
    }
    if(inLen % MAX_BLOCK != 0){
        chunks.push_back(new std::uint8_t[inLen % MAX_BLOCK]);
        memcpy(chunks[i], (unsigned char*)in + i * MAX_BLOCK, inLen % MAX_BLOCK);
    }
    size_t count = 0;
    std::vector<std::uint8_t> outData; 
    bool isFirst = true;
    for(const auto& chunk : chunks){
        count++;
        crypto::EncryptRequest request;
        request.set_sender_id(senderId);
        request.set_receiver_id(receiverId);
        request.set_data(std::string(reinterpret_cast<const char*>(chunk),count == chunks.size() && inLen %MAX_BLOCK? inLen%MAX_BLOCK: MAX_BLOCK ));
        request.set_counter(chunks.size());
        request.set_isfirst(isFirst);
        isFirst = false;
        grpc::ClientContext context;
        grpc::Status status = stub_->encrypt(&context, request, &response);

        if (!status.ok()) {
            log(logger::LogLevel::ERROR,"RPC encrypt failed");
            for (auto ptr : chunks) 
                delete[] ptr;

            return CKR_FUNCTION_FAILED;
        }
        outData.insert(outData.end(), response.encrypted_data().data(), response.encrypted_data().data() + response.encrypted_data().length());
    }
    memcpy((uint8_t *)out, response.signature().data(),
         response.signature().size());
    memcpy((uint8_t*)out + response.signature().size(), outData.data(), outData.size());
    
    for (auto ptr : chunks) 
        delete[] ptr;

    return CKR_OK; 
}


/**
 * @brief Decrypts the input data and extracts the signature.
 * @param senderId The ID of the sender.
 * @param receiverId The ID of the receiver.
 * @param in Pointer to the input data that includes the signature.
 * @param inLen Length of the input data.
 * @param out Pointer where the decrypted output will be stored.
 * @param outLen Reference to the length of the decrypted output.
 * @return CK_RV Status of the operation (CKR_OK on success).
 */
CK_RV CryptoClient::decrypt(int senderId, int receiverId, void *in,
                            size_t inLen, void *out, size_t &outLen) 
{
    size_t encryptBlock = getEncryptedLen(senderId, MAX_BLOCK, false);
    size_t signatureLen = getSignatureLength();                            
    uint8_t *signature = new uint8_t[signatureLen];
    memcpy(signature, in, signatureLen);
    inLen -= signatureLen;
    in = (uint8_t*)in + signatureLen;
    size_t firstBlock = getEncryptedLen(senderId, MAX_BLOCK, true);
    std::vector<std::uint8_t *> chunks;
    size_t i = 0;
    if (inLen > firstBlock) {
       chunks.push_back(new std::uint8_t[firstBlock]);
       memcpy(chunks[i], in , firstBlock);
       i++;
       for (; i < 1 + (inLen - firstBlock) / encryptBlock; i++) {
           chunks.push_back(new std::uint8_t[encryptBlock]);
           memcpy(chunks[i], (uint8_t*)in + ((i - 1) * encryptBlock) + firstBlock,  encryptBlock); 
       }
       if((inLen - firstBlock) % encryptBlock != 0 ){
           chunks.push_back(new std::uint8_t[((inLen - firstBlock) % encryptBlock)]);
           memcpy(chunks[i], (unsigned char *)in + (i > 0? (((i - 1) * encryptBlock) +  firstBlock): 0),
          ((inLen - firstBlock) % encryptBlock));
       }
    }
    else{
      chunks.push_back(new std::uint8_t[inLen]);
      memcpy(chunks[i], (unsigned char *)in,inLen );
    }
    std::vector<std::uint8_t> outData;
    bool isFirst = true;
    size_t count = 0;
    for (const auto &chunk : chunks) {
        count++;
        crypto::DecryptRequest request;
        request.set_sender_id(senderId);
        request.set_receiver_id(receiverId);
        request.set_isfirst(isFirst);
        if(isFirst)
            request.set_encrypted_data(std::string(
           reinterpret_cast<const char *>(chunk),inLen > firstBlock? firstBlock: inLen));
        else 
           request.set_encrypted_data(std::string(
           reinterpret_cast<const char *>(chunk),  (count == chunks.size() && (inLen - firstBlock) % encryptBlock)? (inLen - firstBlock) % encryptBlock: encryptBlock));
        
        isFirst = false;
        request.set_counter(chunks.size());
        std::string signature_str(reinterpret_cast<const char*>(signature), signatureLen);
        request.set_signature(signature_str);
        crypto::DecryptResponse response;
        grpc::ClientContext context;
        grpc::Status status = stub_->decrypt(&context, request, &response);
        if (!status.ok()) {
            log(logger::LogLevel::ERROR,"RPC decrypt failed");
            for (auto ptr : chunks)
                delete[] ptr;
            
            return CKR_FUNCTION_FAILED;
        }

        outData.insert(outData.end(), response.decrypted_data().data(),
                   response.decrypted_data().data() +
                       response.decrypted_data().length());
    }
    outLen = outData.size();
    memcpy(out, outData.data(), outLen);
    for (auto ptr : chunks)
        delete[] ptr;
      
    return CKR_OK;
}