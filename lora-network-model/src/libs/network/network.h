#ifndef NETWORK_H
#define NETWORK_H


#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <cstring>
#include <chrono>
#include <thread>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "../random/random.h"
#include "../random/uniform.h"
#include "../random/normal.h"
#include "../random/clouds.h"
#include "../random/custom.h"
#include "../network/gateway.h"
#include "../network/enddevice.h"

#define DEBUG_MODE 1
#define LINE_BUFFER_SIZE 256 // Line buffer for input file
#define FILE_COLS 9 // Number of columns of input file

enum POS_DIST { UNIFORM = 0, NORMAL = 1, CLOUDS = 2, EXT_POS = 3 };
enum PERIOD_DIST { SOFT = 0, MEDIUM = 1, HARD = 2, EXT_PER = 3 };

using namespace std;

class Network {
    public:
        class Builder { // Creates the network of end devices and basic configuration
            public:
                Builder* setPositionGenerator(POS_DIST posDist);
                Builder* setPeriodGenerator(PERIOD_DIST periodDist);
                Builder* setMapSize(unsigned int mapSize);
                Builder* setNetworkSize(unsigned int networkSize);
                Builder* setMaxSF(unsigned char maxSF);
                Network build(); // Returns the list of end devices

                unsigned int mapSize = 1000; // Size of the map 
                unsigned char maxSF = 12; // Maximum spreading factor
                POS_DIST posDist = UNIFORM; // Distribution of positions
                PERIOD_DIST periodDist = SOFT; // Distribution of periods
                vector<EndDevice*> enddevices; // List of enddevices
                unsigned int H; // System hyperperiod
                unsigned int lastID; // IDs to identify network nodes
            private:
                unsigned int networkSize = 1000; // Number of end devices       
        };

        Network(Builder builder);
        Network(char* filename);
        ~Network();
        
        // Network management
        void autoConnect(); // Given nodes positions and periods, connect them automatically
        void disconnect(); // Disconnect all nodes
        void createGateway(double x = 0, double y = 0);
        bool removeGateway(unsigned int id);
        void removeIdleGateways(); // Remove gateways without end devices
        void removeAllGateways(); // Remove all gateways except 1 and autoconnect
        
        // Optimization steps
        void stepSprings(); // Improve coverage by moving GW using attraction forces
        void stepRandom(); // Try to improve coverage by randomizing GW positions

        // Network post processing
        void configureGWChannels(); // Assign channels to gateways minimizing the number of channels
        void compSchedulerMinPeriod(); // Compute the time scheduler for each gateway using the minimum period
        void compSchedulerEDF(); // Compute the time scheduler for each gateway using the EDF algorithm
        
        // Network information
        void exportNodesCSV(char *filename);
        void printNetworkStatus(FILE *file);
        void printScheduler(char *filename); // Compute the scheduler to validate time requirements
        unsigned int getNonFeasibleEDs(); // Return the number of end devices that cannot be scheduled
        
        // Getters
        static string getPosDistName(POS_DIST posDist);
        string getPosDistName();
        static string getPeriodDistName(PERIOD_DIST periodDist);
        string getPeriodDistName();
        double getEDCoverage(); // Proportion of connected end devices
        long unsigned int getNCEDCount(); // Not connected end devices
        double getAvgUF(); // Average utilization factor of gateways
        
        inline unsigned int getMapSize() {return this->mapSize;}
        inline unsigned int getEDCount() {return this->enddevices.size();}
        inline unsigned int getGWCount() {return this->gateways.size();}
        inline unsigned char getMaxSF() {return this->maxSF;}
        void setMaxSF(unsigned char maxSF); // Set maximum spreading factor
        inline int getMinChannels() {return this->minChannels;}
        inline vector<EndDevice*>* getEDs() {return &(this->enddevices);}
        inline vector<Gateway*>* getGWs() {return &(this->gateways);}
        
        // Utils
        static inline int gcd(int a, int b) {return b == 0 ? a : gcd(b, a % b);}
        static inline int lcm(int a, int b){return a * b / gcd(a, b);}
        
    private: 
        unsigned int mapSize; // Size of the map (required to create new gateways)
        unsigned int H; // Hyperperiod (is calculated from periods)        
        unsigned int lastID; // IDs to identify network nodes        
        unsigned char maxSF; // Maximum spreading factor (used to create new gateways)

        POS_DIST posDist; // Distribution used for ED positions
        PERIOD_DIST periodDist; // Distribution used for ED periods

        vector<EndDevice*> enddevices; // List of enddevices
        vector<vector<vector<unsigned int>>> scheduler; // Scheduler for each gateway, sf and time slot
        vector<unsigned int> timeUnfeasibleEDs; // List of enddevices that cannot be scheduled (due to time constraints)

        // GW management
        vector<Gateway*> gateways; // List of gateways (generated on run())
        int minChannels = 0; // Minimum number of channels for GWs
};

#endif // NETWORK_H