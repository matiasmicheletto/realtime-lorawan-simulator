#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <chrono>
#include <stdio.h>
#include <string>
#include <time.h>
#include "../network/network.h"

#define DEBUG_MODE 1

using namespace std;

enum STEP_METHOD { RANDOM = 0, SPRINGS = 1 };
enum EXIT_CODE { NOT_RUN = 0, TIMEOUT = 1, MAX_ITER = 2, MAX_COVERAGE = 3, MAX_GW = 4 };

class Optimizer {
    public:
        class Builder { // Creates the network of end devices and basic configuration
            public:
                Builder* setNetwork(Network *network);
                Builder* setMaxIter(unsigned int maxIter);
                Builder* setTimeout(unsigned int timeout);
                Builder* setStepMethod(STEP_METHOD stepMethod);
                Builder* setUpdatePeriod(int updatePeriod);
                Optimizer build(); // Returns the list of end devices

                Network *network; // Network to optimize
                unsigned int maxIter = 100; // Maximum number of iterations
                unsigned int timeout = 60; // Timeout in seconds
                STEP_METHOD stepMethod = SPRINGS; // Step method
                int updatePeriod = 10; // Number of iterations between each update call
        };

        Optimizer(Builder builder);
        ~Optimizer();
        
        // Compute the minimun number of gateways to connect all end devices
        void run( void (*progressCallback)(Network *network) = NULL );
        
        // Display results 
        void printResults();
        void exportFullResults(char *filename);
        void appendToLog(char *filename);

        inline unsigned int getIterations() { return this->currentIter; }
        inline unsigned long int getElapsed() { return this->elapsed; }
        inline EXIT_CODE getExitCode() { return this->exitCode; }

        // Utils
        template <typename T> 
        static inline T mclamp(T value, T min, T max) {return value < min ? min : (value > max ? max : value);}
        static inline int gcd(int a, int b) {return b == 0 ? a : gcd(b, a % b);}
        static inline int lcm(int a, int b){return a * b / gcd(a, b);}
        
    private: 
        Network *network; // Network to optimize

        // Optimization parameters
        unsigned int maxIter; // Maximum number of iterations
        unsigned int currentIter; // Current iteration
        unsigned int timeout; // Timeout in seconds
        unsigned long int elapsed; // Elapsed time in milliseconds
        STEP_METHOD stepMethod; // Step method
        int updatePeriod; // Number of iterations between each update call
        EXIT_CODE exitCode; // Exit code
};

#endif // OPTIMIZER_H