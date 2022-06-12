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
        vector<Gateway*> gateways;
        vector<EndDevice*> enddevices;
        int mapSize;
        void autoConnect();
        void step();

    public:
        LoraNetwork(int networkSize, int mapSize);
        ~LoraNetwork();
        void run(int iters, int timeout);
        void print();
};

#endif