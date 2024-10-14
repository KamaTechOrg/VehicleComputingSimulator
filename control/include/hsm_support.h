#ifndef __HSM_SUPPORT__
#define __HSM_SUPPORT__

#include "global_properties.h"

bool decryptData(void *data, int dataLen, uint32_t senderId);

bool encryptData(const void *data, int dataLen, uint8_t *encryptedData,
                 size_t encryptedLength, uint32_t receiverId);

#endif // __HSM_SUPPORT__