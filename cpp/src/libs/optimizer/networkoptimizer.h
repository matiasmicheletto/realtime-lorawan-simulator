#ifndef NETWORKOPTIMIZER_H
#define NETWORKOPTIMIZER_H

#include <vector>
#include <algorithm>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <chrono>

#include "../random/random.h"
#include "../random/uniform.h"
#include "../network/gateway.h"
#include "../network/enddevice.h"

using namespace std;

enum STEP_METHOD { RANDOM, SPRINGS };

class NetworkOptimizer {
    public:
        class Builder { // Creates the network of end devices
            public:
                Builder* setPositionGenerator(Random *posGenerator);
                Builder* setPeriodGenerator(Random *periodGenerator);
                Builder* setMapSize(unsigned int mapSize);
                Builder* setNetworkSize(unsigned int networkSize);
                Builder* setMaxIter(unsigned int maxIter);
                Builder* setTimeout(int timeout);
                Builder* setStepMethod(STEP_METHOD stepMethod);
                NetworkOptimizer build(); // Returns the list of end devices

                vector<EndDevice*> enddevices; // List of enddevices
                unsigned int mapSize = 1000; // Size of the map 
                unsigned int H; // System hyperperiod
                unsigned int lastID; // IDs to identify network nodes
                unsigned int maxIter = 100; // Maximum number of iterations
                int timeout = 60; // Timeout in seconds
                STEP_METHOD stepMethod = SPRINGS; // Step method
            private:
                unsigned int networkSize = 1000; // Number of end devices
                // TODO: set defaults:
                Random *posGenerator; // Generator of random positions for end devices
                Random *periodGenerator; // Generator of periods for end devices
        };

        NetworkOptimizer(Builder builder);
        ~NetworkOptimizer();

        // Compute the minimun number of gateways to connect all end devices
        void run();
        // Display results 
        void printStatus();

        template <typename T> 
        static inline T mclamp(T value, T min, T max) {return value < min ? min : (value > max ? max : value);}
        static inline int gcd(int a, int b) {return b == 0 ? a : gcd(b, a % b);}
        static inline int lcm(int a, int b){return a * b / gcd(a, b);}

    private: 
        vector<EndDevice*> enddevices; // List of enddevices
        unsigned int mapSize; // Size of the map (required to create new gateways)
        unsigned int H; // Hyperperiod (is calculated from periods)        
        unsigned int lastID; // IDs to identify network nodes        

        // GW management
        vector<Gateway*> gateways; // List of gateways (generated on run())
        bool availableChannels[16]; // List of available channels (limits number of gw)
        bool createGateway(double x, double y);
        bool removeGateway(Gateway *gw);
        
        // Network management
        void autoConnect(); // Given nodes positions and periods, connect them automatically
        void disconnect(); // Disconnect all nodes

        // Optimization parameters
        unsigned int maxIter; // Maximum number of iterations
        int timeout; // Timeout in seconds

        void step(); // Improve coverage
        void stepSprings(); // Improve coverage by moving GW
        void stepRandom(); // Improve coverage by randomizing GW positions

        STEP_METHOD stepMethod; // Step method

        // Status
        double getEDCoverage();

};

#endif