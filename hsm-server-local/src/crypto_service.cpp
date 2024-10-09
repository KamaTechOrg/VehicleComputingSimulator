#include "../include/crypto_service.h"
#include "../include/general.h"
#include "../include/crypto_api.h"
#include "../include/debug_utils.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <grpcpp/support/status.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include "../include/debug_utils.h"

grpc::Status CryptoServiceServer::bootSystem(grpc::ServerContext* context, const crypto::BootSystemRequest* request, crypto::Empty* response) 
{
    std::map<int,std::vector<KeyPermission>> usersIdsPermissions;
    for(auto user:request->usersidspermissions())
        for(auto permission: user.permissions())
        usersIdsPermissions[user.userid()].push_back(static_cast<KeyPermission>(permission));
    if(::bootSystem(usersIdsPermissions) == CKR_OK)
        return grpc::Status::OK;
    return grpc::Status::CANCELLED;
}

grpc::Status CryptoServiceServer::addProccess(grpc::ServerContext* context, const crypto::AddProcessRequest* request, crypto::Empty* response){
 
    std::vector<KeyPermission> permissions;
    for(auto permission: request->permissions())
        permissions.push_back(static_cast<KeyPermission>(permission));
 if(::addProccess(request->userid(),permissions) == CKR_OK)
        return grpc::Status::OK;
    return grpc::Status::CANCELLED;
}

grpc::Status CryptoServiceServer::configure(grpc::ServerContext* context, const crypto::ConfigureRequest* request, crypto::Empty* response)
{
        CryptoConfig config(static_cast<SHAAlgorithm>(request->config().hashfunction()),static_cast<AESKeyLength>(request->config().aeskeylength()),
    static_cast<AESChainingMode>(request->config().aeschainingmode()),static_cast<AsymmetricFunction>(request->config().asymmetricfunction()));
     if(::configure(request->userid(),config) == CKR_OK)
        return grpc::Status::OK;
    return grpc::Status::CANCELLED;
}

/**
 * Encrypts the provided data and returns the encrypted result and signature.
 *
 * @param context The gRPC server context.
 * @param request The request containing the data to encrypt.
 * @param response The response containing the encrypted data and signature.
 * @return The gRPC status indicating success or failure.
 */
grpc::Status CryptoServiceServer::encrypt(grpc::ServerContext* context, const crypto::EncryptRequest* request, crypto::EncryptResponse* response)
{
    std::string data = request->data();
    size_t outLen = ::getEncryptedLen(request->sender_id(), request->data().length(), request->isfirst());
    void *out = new uint8_t[outLen];
    size_t signatureLen = ::getSignatureLength();
    void* signature = new uint8_t[signatureLen];
    CK_RV status = ::encrypt(request->sender_id(), request->receiver_id(), static_cast<void*>(const_cast<char*>(request->data().data())), request->data().length(), out, outLen, signature, signatureLen, request->counter());
    if(status != CKR_OK)
        return grpc::Status::CANCELLED;
    char *charPtr = static_cast<char *>(out);
    response->set_encrypted_data(out, outLen);
    response->set_signature(signature, signatureLen);

    delete [] (uint8_t*)out;

    return grpc::Status::OK;

}

/**
 * Decrypts the provided encrypted data and returns the decrypted result.
 *
 * @param context The gRPC server context.
 * @param request The request containing the encrypted data and signature.
 * @param response The response containing the decrypted data.
 * @return The gRPC status indicating success or failure.
 */
grpc::Status CryptoServiceServer::decrypt(grpc::ServerContext* context, const crypto::DecryptRequest* request, crypto::DecryptResponse* response)
{
    size_t outLen = ::getDecryptedLen(request->sender_id(), request->encrypted_data().length(), request->isfirst());
    void* out = new uint8_t[outLen];
    CK_RV status = ::decrypt(request->sender_id(), request->receiver_id(), static_cast<void*>(const_cast<char*>(request->encrypted_data().data())), request->encrypted_data().length(), static_cast<void*>(const_cast<char*>(request->signature().data())), request->signature().length(), out, outLen, request->counter());
   if(status != CKR_OK)
       return grpc::Status::CANCELLED;
    response->set_decrypted_data(out, outLen);
    delete [] (uint8_t*)out;
    return grpc::Status::OK;
}

/**
 * Generates an AES key with the specified permissions and user ID.
 *
 * @param context The gRPC server context.
 * @param request The request containing the user ID and key length.
 * @param response The response containing the generated AES key.
 * @return The gRPC status indicating success or failure.
 */
grpc::Status CryptoServiceServer::generateAESKey(grpc::ServerContext *context, const crypto::GenerateAESKeyRequest *request, crypto::GenerateAESKeyResponse *response) 
{
    std::vector<KeyPermission> permissions;
    for (auto permission : request->permissions())
        permissions.push_back(static_cast<KeyPermission>(permission));
    response->set_aes_key(::generateAESKey(request->user_id(), static_cast<AESKeyLength>(request->keylength()), permissions, request->destuserid()));
    return grpc::Status::OK;
}

/**
 * Generates an RSA key pair with the specified permissions and user ID.
 *
 * @param context The gRPC server context.
 * @param request The request containing the user ID and permissions.
 * @param response The response containing the generated RSA key pair.
 * @return The gRPC status indicating success or failure.
 */
grpc::Status CryptoServiceServer::generateRSAKeyPair(grpc::ServerContext* context, const crypto::GenerateKeyPairRequest* request, crypto::GenerateKeyPairResponse* response)
{
    std::vector<KeyPermission> permissions;
    for(auto permission: request->permissions())
        permissions.push_back(static_cast<KeyPermission>(permission));
    std::pair<std::string, std::string> pair = ::generateRSAKeyPair(request->user_id(), permissions);
    response->set_private_key(pair.second);
    response->set_public_key(pair.first);
    return grpc::Status::OK;
}

/**
 * Generates an ECC key pair with the specified permissions and user ID.
 *
 * @param context The gRPC server context.
 * @param request The request containing the user ID and permissions.
 * @param response The response containing the generated ECC key pair.
 * @return The gRPC status indicating success or failure.
 */
grpc::Status CryptoServiceServer::generateECCKeyPair(grpc::ServerContext* context, const crypto::GenerateKeyPairRequest* request, crypto::GenerateKeyPairResponse* response)
{
    std::vector<KeyPermission> permissions;
    for(auto permission: request->permissions())
        permissions.push_back(static_cast<KeyPermission>(permission));
    std::pair<std::string, std::string> pair = ::generateECCKeyPair(request->user_id(), permissions);
    response->set_private_key(pair.second);
    response->set_public_key(pair.first);
    return grpc::Status::OK;
}

/**
 * Retrieves the length of signed data (signature length).
 *
 * @param context The gRPC server context.
 * @param request The request for getting the hash length.
 * @param response The response containing the length of the signed data.
 * @return The gRPC status indicating success or failure.
 */
grpc::Status CryptoServiceServer::getSignedDataLength(grpc::ServerContext* context, const crypto::GetHashLengthRequest* request, crypto::GetLengthResponse* response)
{ 
    response->set_len(::getSignatureLength());
    return grpc::Status::OK;
}

// gRPC server methods for cryptographic operations

// Retrieves the public ECC key associated with a given user ID.
grpc::Status CryptoServiceServer::getPublicECCKeyByUserId(grpc::ServerContext* context, const crypto::KeyRequest* request, crypto::KeyResponse* response)
{
    response->set_key(::getPublicECCKeyByUserId(request->user_id()));
    return grpc::Status::OK;
}

// Retrieves the public RSA key associated with a given user ID.
grpc::Status CryptoServiceServer::getPublicRSAKeyByUserId(grpc::ServerContext* context, const crypto::KeyRequest* request, crypto::KeyResponse* response)
{
    response->set_key(::getPublicRSAKeyByUserId(request->user_id()));
    return grpc::Status::OK;
}

// Gets the length of the encrypted ECC data.
grpc::Status CryptoServiceServer::getECCencryptedLength(grpc::ServerContext* context, const crypto::GetLengthRequest* request, crypto::GetLengthResponse* response)
{
    response->set_len(::getECCencryptedLength());
    return grpc::Status::OK;
}

// Gets the length of the decrypted ECC data.
grpc::Status CryptoServiceServer::getECCDecryptedLength(grpc::ServerContext* context, const crypto::GetLengthRequest* request, crypto::GetLengthResponse* response)
{
    response->set_len(::getECCdecryptedLength());
    return grpc::Status::OK;
}

// Encrypts data using ECC with the specified sender ID and key ID.
grpc::Status CryptoServiceServer::ECCencrypt(grpc::ServerContext* context, const crypto::AsymetricEncryptRequest* request, crypto::AsymetricEncryptResponse* response)
{
    size_t outLen = ::getECCencryptedLength();
    void* out = new uint8_t[outLen];
    
    ::ECCencrypt(request->senderid(), request->keyid(), (void*)request->data().data(), request->data().length(), out, outLen);
    response->set_encrypted_data(out, outLen);
    delete [] (uint8_t*)out;
    return grpc::Status::OK;
}

// Decrypts ECC-encrypted data using the specified receiver ID and key ID.
grpc::Status CryptoServiceServer::ECCdecrypt(grpc::ServerContext* context, const crypto::AsymetricDecryptRequest* request, crypto::AsymetricDecryptResponse* response)
{
    size_t outLen = ::getECCdecryptedLength();
    void* out = new uint8_t[outLen];
    ::ECCdecrypt(request->receiverid(), request->keyid(), (void*)request->data().data(), request->data().length(), out, outLen);
    response->set_decrypted_data(out, outLen);
    delete [] (uint8_t*)out;
    return grpc::Status::OK;
}

// Gets the length of the encrypted RSA data.
grpc::Status CryptoServiceServer::getRSAencryptedLength(grpc::ServerContext* context, const crypto::GetLengthRequest* request, crypto::GetLengthResponse* response)
{
    response->set_len(::getRSAencryptedLength());
    return grpc::Status::OK;
}

// Gets the length of the decrypted RSA data.
grpc::Status CryptoServiceServer::getRSAdecryptedLength(grpc::ServerContext* context, const crypto::GetLengthRequest* request, crypto::GetLengthResponse* response)
{
    response->set_len(::getRSAdecryptedLength());
    return grpc::Status::OK;
}

// Encrypts data using RSA with the specified sender ID and key ID.
grpc::Status CryptoServiceServer::RSAencrypt(grpc::ServerContext* context, const crypto::AsymetricEncryptRequest* request, crypto::AsymetricEncryptResponse* response)
{
    size_t outLen = ::getRSAencryptedLength();
    void* out = new uint8_t[outLen];
    
    ::RSAencrypt(request->senderid(), request->keyid(), (void*)request->data().data(), request->data().length(), out, outLen);
    response->set_encrypted_data(out, outLen);
        delete [] (uint8_t*)out;

    return grpc::Status::OK;
}

// Decrypts RSA-encrypted data using the specified receiver ID and key ID.
grpc::Status CryptoServiceServer::RSAdecrypt(grpc::ServerContext* context, const crypto::AsymetricDecryptRequest* request, crypto::AsymetricDecryptResponse* response)
{
    size_t outLen = ::getRSAdecryptedLength();
    void* out = new uint8_t[outLen];
    ::RSAdecrypt(request->receiverid(), request->keyid(), (void*)request->data().data(), request->data().length(), out, &outLen);
    response->set_decrypted_data(out, outLen);
    delete [] (uint8_t*)out;

    return grpc::Status::OK;
}


// aes
/**
 * Retrieves the length of encrypted data using AES.
 *
 * @param context The gRPC server context.
 * @param request The request containing data length and chaining mode.
 * @param response The response object to send back the length.
 * @return grpc::Status indicating the success or failure of the operation.
 */
grpc::Status CryptoServiceServer::getAESencryptedLength(
    grpc::ServerContext *context, const crypto::GetAESLengthRequest *request,
    crypto::GetLengthResponse *response) 
{
  response->set_len(::getAESencryptedLength(
      request->datalen(), request->isfirst(),
      static_cast<AESChainingMode>(request->chainingmode())));

  return grpc::Status::OK;
}

/**
 * Retrieves the length of decrypted data using AES.
 *
 * @param context The gRPC server context.
 * @param request The request containing data length and chaining mode.
 * @param response The response object to send back the length.
 * @return grpc::Status indicating the success or failure of the operation.
 */
grpc::Status CryptoServiceServer::getAESdecryptedLength(
    grpc::ServerContext *context, const crypto::GetAESLengthRequest *request,
    crypto::GetLengthResponse *response) 
{
  response->set_len(
      ::getAESdecryptedLength(request->datalen(), request->isfirst(), static_cast<AESChainingMode>(request->chainingmode())));

  return grpc::Status::OK;
}

/**
 * Encrypts data using AES encryption.
 *
 * @param context The gRPC server context.
 * @param request The request containing sender and receiver IDs and data.
 * @param response The response object to send back the encrypted data.
 * @return grpc::Status indicating the success or failure of the operation.
 */
grpc::Status CryptoServiceServer::AESencrypt(grpc::ServerContext *context,
                                const crypto::AESEncryptRequest *request,
                                crypto::AESEncryptResponse *response) 
{
    size_t outLen = ::getAESencryptedLength(
        request->data().length(), request->isfirst(),
        static_cast<AESChainingMode>(request->chainingmode()));

    std::unique_ptr<uint8_t[], std::default_delete<uint8_t[]>> out(new uint8_t[outLen]);

    CK_RV status = ::AESencrypt(request->sender_id(), request->receiver_id(),
                 (void *)request->data().data(), request->data().length(),
                 out.get(), outLen,
                 static_cast<AESKeyLength>(request->key_length()),
                 static_cast<AESChainingMode>(request->chainingmode()),
                 request->counter(), request->key_id());
    response->set_encrypted_data(out.get(), outLen);

    return grpc::Status::OK;
}


/**
 * Decrypts data using AES decryption.
 *
 * @param context The gRPC server context.
 * @param request The request containing sender and receiver IDs and encrypted data.
 * @param response The response object to send back the decrypted data.
 * @return grpc::Status indicating the success or failure of the operation.
 */
grpc::Status CryptoServiceServer::AESdecrypt(grpc::ServerContext *context,
                                const crypto::AESDecryptRequest *request,
                                crypto::AESDecryptResponse *response) 
{
    size_t outLen = ::getAESdecryptedLength(request->data_in().length(), request->isfirst(), static_cast<AESChainingMode>(request->chainingmode()));
    
    std::unique_ptr<uint8_t[]> out(new uint8_t[outLen]);
    CK_RV status = ::AESdecrypt(request->sender_id(), request->receiver_id(),
                 (void *)request->data_in().data(), request->data_in().length(),
                 out.get(), outLen, static_cast<AESKeyLength>(request->key_length()),
                 static_cast<AESChainingMode>(request->chainingmode()),
                 request->counter(), request->key_id());
    if(status != CKR_OK)
        return grpc::Status::CANCELLED;
    response->set_decrypted_data(out.get(), outLen);
    return grpc::Status::OK;
}

/**
 * Retrieves the length of encrypted data for a specific sender.
 *
 * @param context The gRPC server context.
 * @param request The request containing sender ID and input length.
 * @param response The response object to send back the length.
 * @return grpc::Status indicating the success or failure of the operation.
 */
grpc::Status CryptoServiceServer::getEncryptedLen(grpc::ServerContext* context, const crypto::GetWholeLength* request, crypto::GetLengthResponse* response)
{
    response->set_len(::getEncryptedLen(request->senderid(), request->inlen(), request->isfirst()));
    
    return grpc::Status::OK;
}

/**
 * Retrieves the length of decrypted data for a specific sender.
 *
 * @param context The gRPC server context.
 * @param request The request containing sender ID and input length.
 * @param response The response object to send back the length.
 * @return grpc::Status indicating the success or failure of the operation.
 */
grpc::Status CryptoServiceServer::getDecryptedLen(grpc::ServerContext* context, const crypto::GetWholeLength* request, crypto::GetLengthResponse* response)
{
    response->set_len(::getDecryptedLen(request->senderid(), request->inlen(), request->isfirst()));
    
    return grpc::Status::OK;
}

/**
 * Updates the signing process with new data.
 *
 * @param context The gRPC server context.
 * @param request The request containing sender ID and data to sign.
 * @param response The response object to send back the status of the operation.
 * @return grpc::Status indicating the success or failure of the operation.
 */
grpc::Status CryptoServiceServer::signUpdate(grpc::ServerContext* context, const crypto::SignRequest* request, crypto::SignResponse* response)
{
    CK_RV status = ::signUpdate(request->sender_id(), (void*)request->data().data(), request->data().length(), static_cast<SHAAlgorithm>(request->hash_func()), request->counter());
    return status == CKR_OK ? grpc::Status::OK : grpc::Status::CANCELLED;
}

/**
 * Finalizes the signing process and returns the signature.
 *
 * @param context The gRPC server context.
 * @param request The request containing sender ID and key ID.
 * @param response The response object to send back the generated signature.
 * @return grpc::Status indicating the success or failure of the operation.
 */
grpc::Status CryptoServiceServer::signFinalize(grpc::ServerContext* context, const crypto::SignRequest* request, crypto::SignResponse* response)
{
    size_t signatureLen = ::getSignatureLength();
    void * signature  = new uint8_t[signatureLen];
    if(CK_RV status = ::signFinalize(request->sender_id(), signature, signatureLen, static_cast<SHAAlgorithm>(request->hash_func()), request->key_id()) != CKR_OK)
           return grpc::Status::CANCELLED;
    response->set_signature(signature, getSignatureLength());
    delete [] (uint8_t*)signature;

    return grpc::Status::OK;
}

/**
 * Updates the verification process with new data.
 *
 * @param context The gRPC server context.
 * @param request The request containing receiver ID and data to verify.
 * @param response The response object to send back the status of the verification.
 * @return grpc::Status indicating the success or failure of the operation.
 */
grpc::Status CryptoServiceServer::verifyUpdate(grpc::ServerContext* context, const crypto::VerifyRequest* request, crypto::VerifyResponse* response)
{
    if (::verifyUpdate(request->receiver_id(), (void*)request->data().data(), request->data().length(), static_cast<SHAAlgorithm>(request->hash_func()), request->counter()) == CKR_OK)
        return grpc::Status::OK;

    return grpc::Status::CANCELLED;
}

// Finalizes the verification process of a signature.
// Checks if the provided signature is valid by comparing it against the expected value.
// If the verification is successful, sets the validity of the signature in the response to true.
// 
// Parameters:
// - context: A pointer to the server context.
// - request: A pointer to the VerifyRequest containing the necessary information for verification,
//   including receiver ID, signature data, hash function, and key ID.
// - response: A pointer to the VerifyResponse where the result of the verification will be stored.
//
// Returns:
// - grpc::Status::OK if the signature is valid.
// - grpc::Status::CANCELLED if the verification fails.
grpc::Status CryptoServiceServer::verifyFinalize(grpc::ServerContext* context, const crypto::VerifyRequest* request, crypto::VerifyResponse* response)
{
    if(::verifyFinalize(request->receiver_id(), (void*)request->signature().data(),request->signature().length(), static_cast<SHAAlgorithm>(request->hash_func()), request->key_id())!=CKR_OK)
        return grpc::Status::CANCELLED;
    response->set_valid(true);
    return grpc::Status::OK;
}

void RunServer() {
    std::string serverAddress("0.0.0.0:50051");
    CryptoServiceServer service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << serverAddress << std::endl;

    server->Wait();
    
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}
