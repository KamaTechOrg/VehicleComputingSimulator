#ifndef __HSM_SUPPORT__
#define __HSM_SUPPORT__
#include <stddef.h>
#include <cstdint>
namespace hsm {
bool decryptData(void *data, uint32_t senderId, uint32_t myId);
bool encryptData(const void *data, int dataLen, uint8_t *encryptedData,
                 size_t encryptedLength, uint32_t myId, uint32_t receiverId);
size_t getEncryptedLen(uint32_t myId, size_t dataLength);
}  // namespace hsm
#endif  // __HSM_SUPPORT__