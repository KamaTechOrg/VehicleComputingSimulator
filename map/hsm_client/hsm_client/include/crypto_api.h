#ifndef CRYPTO_CLIENT_H
#define CRYPTO_CLIENT_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "../proto/encryption.grpc.pb.h"
#include "general.h"
#include "../proto/encryption.pb.h"

class CryptoClient {
public:
//ctor
CryptoClient(std::shared_ptr<grpc::Channel> channel)
      : stub_(crypto::CryptoService::NewStub(channel)) {}
CryptoClient()
      : stub_(crypto::CryptoService::NewStub(grpc::CreateChannel("10.0.0.4:50051", grpc::InsecureChannelCredentials()))) {}
//config
CK_RV configure(int userId, CryptoConfig config);
CK_RV bootSystem(const std::map<int,std::vector<KeyPermission>>
&usersIdspermissions); 
CK_RV addProccess(
  int userId, std::vector<KeyPermission> &permissions);
//generate key
std::string generateAESKey(int userId, AESKeyLength aesKeyLength,
                           std::vector<KeyPermission> permissions,
                           int destUserId);
std::pair<std::string, std::string>
generateRSAKeyPair(int userId, std::vector<KeyPermission> permissions);
std::pair<std::string, std::string>
generateECCKeyPair(int userId, std::vector<KeyPermission> permissions);
// sign-verify
size_t getSignatureLength();
size_t getSignedDataLength(size_t inLen);
size_t getVerifiedDataLength(size_t inLen);
CK_RV sign(int senderId, void *in, size_t inLen, uint8_t *&out, size_t &outLen,
           SHAAlgorithm hashFunc, std::string keyId);
CK_RV verify(int senderId, int recieverId, void *in, size_t inLen, void *out,
             size_t &outLen, SHAAlgorithm hashFunc, std::string keyId);
// get public key id
std::string getPublicECCKeyByUserId(int userId);
std::string getPublicRSAKeyByUserId(int userId);
// ecc
size_t getECCencryptedLength(size_t dataLen);
size_t getECCdecryptedLength(size_t dataLen);
CK_RV ECCencrypt(int senderId, std::string keyId, void *in, size_t inLen,
                 void *out, size_t &outLen);
CK_RV ECCdecrypt(int receiverId, std::string keyId, void *in, size_t inLen,
                 void *out, size_t &outLen);
// rsa
size_t getRSAencryptedLength();
size_t getRSAdecryptedLength();
CK_RV RSAencrypt(int userId, std::string keyId, void *in, size_t inLen,
                 void *out, size_t outLen);
CK_RV RSAdecrypt(int userId, std::string keyId, void *in, size_t inLen,
                 void *out, size_t *outLen);
// aes
size_t getAESencryptedLength(size_t dataLen, bool isFirst, AESChainingMode chainingMode);
size_t getAESdecryptedLength(size_t dataLen, bool isFirst, AESChainingMode chainingMode);
size_t getAESencryptedLengthClient(size_t dataLen, AESChainingMode chainingMode);
size_t getAESdecryptedLengthClient(size_t dataLen, AESChainingMode chainingMode);
CK_RV AESencrypt(int senderId, int recieverId, void *in, size_t inLen,
                 void *&out, unsigned int &outLen, AsymmetricFunction func,
                 AESKeyLength keyLength, AESChainingMode chainingMode,
                 std::string keyId);
CK_RV AESdecrypt(int senderId, int recieverId, uint8_t *in, size_t inLen,
                               uint8_t *&out, size_t &outLen,
                               AsymmetricFunction func,
                               AESKeyLength aesKeyLength, AESChainingMode chainingMode,
                               std::string keyId);
// encrypt-decrypt
size_t getEncryptedLen(int senderId, size_t inLen, bool isfirst);
size_t getDecryptedLen(int senderId, size_t encryptedLength, bool isfirst);
size_t getDecryptedLenClient(int senderId, size_t encryptedLengt);
size_t getEncryptedLenClient(int senderId, size_t encryptedLength);

CK_RV encrypt(int senderId, int receiverId, void *in, size_t inLen, void *out,
              size_t &outLen);
CK_RV decrypt(int senderId, int receiverId, void *in, size_t inLen, void *out,
              size_t &outLen);

private:
    std::unique_ptr<crypto::CryptoService::Stub> stub_;
    static const size_t MAX_BLOCK = 1024; 
    static const size_t IV_SIZE = 16;
};

#endif // CRYPTO_CLIENT_H