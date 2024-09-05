#include "ecc.h"
#ifndef __KEY_MANAGER_H__
#define __KEY_MANAGER_H__

#include <gmpxx.h>

mpz_class getECCPrivateKey(int id);

Point getECCPublicKey(int id);

#endif  //__KEY_MANAGER_H__