#ifndef GATEWAY_H
#define GATEWAY_H

class EndDevice; // Forward declaration

#include <vector>
#include <math.h>
#include "node.h"
#include "enddevice.h"

using namespace std;

class Gateway : public Node {
    private: 
        void resetSlots();
        bool useSlots(unsigned char sf, unsigned int period);
        void freeSlots(unsigned char sf, unsigned int period);

        unsigned char getMinSF(double distance);
        unsigned char getMaxSF(unsigned int period);

        vector<EndDevice*> connectedEDs;
        unsigned int H;
        double maxSlots;
        bool availableChannels[16];
        unsigned int availableSlots[6];

    public:
        Gateway(double x, double y, unsigned int id, unsigned int hyperperiod);
        ~Gateway();

        bool connectTo(EndDevice *ed);
        bool disconnectFrom(EndDevice *ed);
        void disconnect(); 
        double getUF();
};

#endif