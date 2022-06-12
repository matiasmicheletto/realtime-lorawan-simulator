#ifndef GATEWAY_H
#define GATEWAY_H

#include "node.h"

class Gateway : public Node {
    private: 
        int *enddevices;

    public:
        Gateway(int x, int y);
        ~Gateway();
};

#endif