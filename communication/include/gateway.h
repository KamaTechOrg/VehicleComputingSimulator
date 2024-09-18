#ifndef GATEWAY_H
#define GATEWAY_H

#include <cstdint>
#include <string>
#include "packet.h"

class Gateway {
public:
    Gateway();
    void translate(Packet &packet);
};

#endif // GATEWAY_H
