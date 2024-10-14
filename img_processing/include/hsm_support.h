#ifndef __HSM_SUPPORT__
#define __HSM_SUPPORT__
#include <cstdint>
#include <cstddef>
bool decryptData(void *data, int dataLen, uint32_t senderId,uint32_t myId);

bool encryptData(const void *data, int dataLen, uint8_t *encryptedData,
                 size_t encryptedLength, uint32_t receiverId,uint32_t myId);

#endif // __HSM_SUPPORT__