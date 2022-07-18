#ifndef GATEWAY_H
#define GATEWAY_H

class EndDevice; // Forward declaration

#include <vector>
#include <math.h>
#include "node.h"
#include "enddevice.h"
#include "stdio.h"

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

        // GW dynamic position management
        inline void setVel(double vx, double vy) { this->velX = vx; this->velY = vy; }
        inline void addToVel(double vx, double vy) { this->velX += vx; this->velY += vy; }
        inline void getVel(double &vx, double &vy) { vx = this->velX; vy = this->velY; }
        void updatePos(double vlim = 0.0);

        // Network management
        bool addEndDevice(EndDevice *ed); // Try to connect an end device
        bool removeEndDevice(EndDevice *ed); // Disconnects from end device
        void disconnect(); // Disconnect from all end devices                
        
        // Maximum spreading factor allowed for this GW
        inline void setMaxSF(unsigned char maxSF) { this->maxSF = maxSF < 7 ? 7 : (maxSF > 12 ? 12 : maxSF); }; // Configure different SF
        inline void reduceMaxSF() { this->maxSF = this->maxSF == 7 ? 7 : this->maxSF - 1;}
         
        // Operating channel
        inline void setChannel(unsigned char channel){ this->channel = channel; }
        inline unsigned char getChannel() { return this->channel; } 

        // Stats
        double getUF(); // GW current utilization factor
        vector<double> getUFbySF(); // GW current utilization factor by spreading factor
        unsigned int connectedEDsCount(); // Number of connected end devices
        vector<EndDevice*> getConnectedEDs(unsigned char sf); // List of connected end devices using sf
        static double getRange(const unsigned char sf); // Operating range depends on the spreading factor
        double getRange(); // Range of this GW        
        static unsigned char getMinSF(double distance); // Returns minimum SF for connecting to an end device at given distance
        static unsigned char getMaxSF(unsigned int period); // Returns maximum SF for connecting to an end device with given period

        template <typename T> 
        static inline T mclamp(T value, T min, T max) {return value < min ? min : (value > max ? max : value);}

    private: 
        void resetUF(); // Restores available slots to initial values
        bool allocate(unsigned char sf, unsigned int period); // Try to use slots for given sf
        void deallocate(unsigned char sf, unsigned int period); // Restores slots for given sf

        double velX = 0.0, velY = 0.0; // GW velocity

        unsigned int H;        
        unsigned char channel;        
        double UF[6]; // Spreading factor utilization factor
        unsigned char maxSF;
        vector<EndDevice*> connectedEDs; // List of connected end devices
};

#endif