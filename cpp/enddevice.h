#ifndef ENDDEVICE_H
#define ENDDEVICE_H

#include "node.h"
#include "gateway.h"

class EndDevice : public Node {
    private:
        int period;
        
    public: 
        EndDevice(int x, int y, int period);
        ~EndDevice();
        void connectTo(Gateway *gw);
};

#endif