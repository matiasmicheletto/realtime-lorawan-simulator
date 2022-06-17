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
        unsigned int maxSlots;
        unsigned char channel;
        unsigned int availableSlots[6];

    public:
        Gateway(
            double x, 
            double y, 
            unsigned int id, 
            unsigned int hyperperiod, 
            unsigned char channel);
        ~Gateway();

        bool addEndDevice(EndDevice *ed);
        bool removeEndDevice(EndDevice *ed);
        void disconnect(); 
        double getUF();
        unsigned int connectedEDsCount();
        inline unsigned char getChannel() { return this->channel; }
        
};

#endif