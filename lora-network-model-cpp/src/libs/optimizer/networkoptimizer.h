#ifndef NETWORKOPTIMIZER_H
#define NETWORKOPTIMIZER_H

#include <vector>
#include <algorithm>
#include <chrono>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "../random/random.h"
#include "../random/uniform.h"
#include "../network/gateway.h"
#include "../network/enddevice.h"

using namespace std;

enum STEP_METHOD { RANDOM = 0, SPRINGS = 1 };
enum EXIT_CODE { NOT_RUN = 0, TIMEOUT = 1, MAX_ITER = 2, MAX_COVERAGE = 3, MAX_GW = 4 };

class NetworkOptimizer {
    public:
        class Builder { // Creates the network of end devices and basic configuration
            public:
                Builder* setPositionGenerator(Random *posGenerator);
                Builder* setPeriodGenerator(Random *periodGenerator);
                Builder* setMapSize(unsigned int mapSize);
                Builder* setNetworkSize(unsigned int networkSize);
                Builder* setMaxIter(unsigned int maxIter);
                Builder* setTimeout(int timeout);
                Builder* setStepMethod(STEP_METHOD stepMethod);
                Builder* setUpdatePeriod(int updatePeriod);
                NetworkOptimizer build(); // Returns the list of end devices

                vector<EndDevice*> enddevices; // List of enddevices
                unsigned int mapSize = 1000; // Size of the map 
                int H; // System hyperperiod
                unsigned int lastID; // IDs to identify network nodes
                unsigned int maxIter = 100; // Maximum number of iterations
                int timeout = 60; // Timeout in seconds
                STEP_METHOD stepMethod = SPRINGS; // Step method
                int updatePeriod = 10; // Number of iterations between each update call
            private:
                unsigned int networkSize = 1000; // Number of end devices
                // TODO: set defaults:
                Random *posGenerator; // Generator of random positions for end devices
                Random *periodGenerator; // Generator of periods for end devices
        };

        NetworkOptimizer(Builder builder);
        ~NetworkOptimizer();
        
        // Compute the minimun number of gateways to connect all end devices
        void run( void (*updateCallback)(vector<Gateway*>*, vector<EndDevice*>*) = NULL );
        // Display results 
        void printStatus(char *filename, bool saveLog);

        template <typename T> 
        static inline T mclamp(T value, T min, T max) {return value < min ? min : (value > max ? max : value);}
        static inline int gcd(int a, int b) {return b == 0 ? a : gcd(b, a % b);}
        static inline int lcm(int a, int b){return a * b / gcd(a, b);}
        
    private: 
        vector<EndDevice*> enddevices; // List of enddevices
        unsigned int mapSize; // Size of the map (required to create new gateways)
        int H; // Hyperperiod (is calculated from periods)        
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
        unsigned int currentIter; // Current iteration
        int timeout; // Timeout in seconds
        int elapsed; // Elapsed time in seconds
        EXIT_CODE exitCode; // Exit code

        void step(); // Improve coverage
        void stepSprings(); // Improve coverage by moving GW
        void stepRandom(); // Improve coverage by randomizing GW positions

        STEP_METHOD stepMethod; // Step method
        int updatePeriod; // Number of iterations between each update call

        // Status
        double getEDCoverage();

};

#endif // NETWORKOPTIMIZER_H