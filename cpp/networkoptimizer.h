#ifndef NETWORKOPTIMIZER_H
#define NETWORKOPTIMIZER_H

#include <vector>
#include <algorithm>
#include <stdio.h>
#include <chrono>
#include "tools.h"
#include "gateway.h"
#include "enddevice.h"

using namespace std;

class NetworkOptimizer {
    protected: 
        vector<Gateway*> gateways; // List of gateways
        vector<EndDevice*> enddevices; // List of enddevices
        unsigned int mapSize; // Size of the map
        unsigned int lastID; // IDs to identify network nodes
        unsigned int H; // Hyperperiod
        bool availableChannels[16]; // List of available channels
        
        // Nodes management
        void createEndDevice(double x, double y, unsigned int period);
        bool removeEndDevice(EndDevice *ed);
        
        bool createGateway(double x, double y);
        bool removeGateway(Gateway *gw);
        
        // Network management
        void autoConnect(); // Given nodes positions and periods, connect them automatically
        void disconnect(); // Disconnect all nodes

        virtual void step(); // Improve coverage (define in child classes)

        // Status
        double getEDCoverage();

    public:
        NetworkOptimizer();
        virtual ~NetworkOptimizer();

        enum ED_DIST {UNIFORM, NORMAL, CLOUD};

        // Add the end devices with their periods to the map
        void init(
            unsigned int networkSize, 
            unsigned int mapSize, 
            ED_DIST posDist,
            const unsigned int *periods,
            const double *prob,
            const int len
        );

        // Compute the minimun number of gateways to connect all end devices
        void minimizeGW(unsigned int iters, unsigned int timeout);
        
        // Displays 
        void printStatus();
};

#endif