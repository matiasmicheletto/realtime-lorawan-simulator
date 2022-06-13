#ifndef LORANETWORK_H
#define LORANETWORK_H

#include <vector>
#include <stdio.h>
#include "tools.h"
#include "gateway.h"
#include "enddevice.h"

using namespace std;

class LoraNetwork {
    private: 
        vector<Gateway*> gateways; // List of gateways
        vector<EndDevice*> enddevices; // List of enddevices
        unsigned int mapSize; // Size of the map
        unsigned int lastID; // IDs to identify network nodes
        unsigned int H; // Hyperperiod
        
        // Nodes management
        void createEndDevice(double x, double y, unsigned int period);
        bool removeEndDevice(EndDevice *ed);
        
        void createGateway(double x, double y);
        bool removeGateway(Gateway *gw);
        
        // Network management
        void autoConnect(); // Given nodes positions and periods, connect them automatically
        void step(); // Improve coverage by moving gateways

    public:
        LoraNetwork();
        ~LoraNetwork();

        enum ED_DIST {UNIFORM, NORMAL, CLOUD};

        void init(
            unsigned int networkSize, 
            unsigned int mapSize, 
            ED_DIST posDist,
            unsigned int *periods,
            double *prob
        );

        void minimizeGW(unsigned int iters, unsigned int timeout);
        void printStatus();
};

#endif