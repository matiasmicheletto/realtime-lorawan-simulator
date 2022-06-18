#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <algorithm>
#include <string>
#include <chrono>
#include <time.h>
#include <stdio.h>

#include "../random/random.h"
#include "../random/uniform.h"
#include "../random/normal.h"
#include "../random/clouds.h"
#include "../random/custom.h"
#include "../network/gateway.h"
#include "../network/enddevice.h"

#define DEBUG_MODE 1

enum POS_DIST { UNIFORM = 0, NORMAL = 1, CLOUDS = 2 };
enum PERIOD_DIST { SOFT = 0, MEDIUM = 1, HARD = 2 };

class Network {
    public:
        class Builder { // Creates the network of end devices and basic configuration
            public:
                Builder* setPositionGenerator(POS_DIST posDist);
                Builder* setPeriodGenerator(PERIOD_DIST periodDist);
                Builder* setMapSize(unsigned int mapSize);
                Builder* setNetworkSize(unsigned int networkSize);
                Network build(); // Returns the list of end devices

                unsigned int mapSize = 1000; // Size of the map 
                int H; // System hyperperiod
                unsigned int lastID; // IDs to identify network nodes
                std::vector<EndDevice*> enddevices; // List of enddevices
                POS_DIST posDist = UNIFORM; // Distribution of positions
                PERIOD_DIST periodDist = SOFT; // Distribution of periods
            private:
                unsigned int networkSize = 1000; // Number of end devices       
        };

        Network(Builder builder);
        ~Network();
        
        // Network management
        void autoConnect(); // Given nodes positions and periods, connect them automatically
        void disconnect(); // Disconnect all nodes
        bool createGateway(double x = 0, double y = 0);
        bool removeGateway(unsigned int id);
        void removeAllGateaways(); // Remove all gateways except 1 and autoconnect
        void stepSprings(); // Improve coverage by moving GW using attraction forces
        void stepRandom(); // Try to improve coverage by randomizing GW positions
        
        // Network information
        double getEDCoverage();
        void exportNodesCSV(char *filename);
        void printNetworkStatus(FILE *file);
        std::string getPosDistName();
        std::string getPeriodDistName();
        inline unsigned int getMapSize() {return this->mapSize;}
        inline unsigned int getEDCount() {return this->enddevices.size();}
        inline unsigned int getGWCount() {return this->gateways.size();}
        inline vector<EndDevice*>* getEDs() {return &(this->enddevices);}
        inline vector<Gateway*>* getGWs() {return &(this->gateways);}
        
        // Utils
        template <typename T> 
        static inline T mclamp(T value, T min, T max) {return value < min ? min : (value > max ? max : value);}
        static inline int gcd(int a, int b) {return b == 0 ? a : gcd(b, a % b);}
        static inline int lcm(int a, int b){return a * b / gcd(a, b);}
        
    private: 
        vector<EndDevice*> enddevices; // List of enddevices
        unsigned int mapSize; // Size of the map (required to create new gateways)
        int H; // Hyperperiod (is calculated from periods)        
        unsigned int lastID; // IDs to identify network nodes        

        POS_DIST posDist; // Distribution used for ED positions
        PERIOD_DIST periodDist; // Distribution used for ED periods

        // GW management
        vector<Gateway*> gateways; // List of gateways (generated on run())
        bool availableChannels[16]; // List of available channels (limits number of gw)
};

#endif // NETWORK_H