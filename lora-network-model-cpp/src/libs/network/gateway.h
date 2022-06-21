#ifndef GATEWAY_H
#define GATEWAY_H

class EndDevice; // Forward declaration

#include <vector>
#include <math.h>
#include "node.h"
#include "enddevice.h"

using namespace std;

class Gateway : public Node {
    public:
        Gateway(
            double x, 
            double y, 
            unsigned int id, 
            unsigned int hyperperiod, 
            unsigned char channel = 0,
            unsigned char maxSF = 12);
        ~Gateway();

        bool addEndDevice(EndDevice *ed); // Try to connect an end device
        bool removeEndDevice(EndDevice *ed); // Disconnects from end device
        void disconnect(); // Disconnect from all end devices
        double getUF(); // GW current utilization factor
        unsigned int connectedEDsCount(); // Number of connected end devices
        double getRange(); // Operating range equals 2000 mts for max. SF 12, or 1000 mts for SF 11 and such
        inline void setMaxSF(unsigned char maxSF) { this->maxSF = maxSF; }; // Configure different SF
        inline void reduceMaxSF() { this->maxSF = this->maxSF == 7 ? 7 : this->maxSF - 1;}
         // Operating channel
        inline void setChannel(unsigned char channel){ this->channel = channel; }
        inline unsigned char getChannel() { return this->channel; } 

    private: 
        void resetSlots(); // Restores available slots to initial values
        bool useSlots(unsigned char sf, unsigned int period); // Try to use slots for given sf
        void freeSlots(unsigned char sf, unsigned int period); // Restores slots for given sf

        unsigned char getMinSF(double distance); // Returns minimum SF for connecting to an end device at given distance
        unsigned char getMaxSF(unsigned int period); // Returns maximum SF for connecting to an end device with given period

        vector<EndDevice*> connectedEDs;
        unsigned int H;
        unsigned int maxSlots;
        unsigned char channel;
        unsigned int availableSlots[6];
        unsigned char maxSF;
};

#endif