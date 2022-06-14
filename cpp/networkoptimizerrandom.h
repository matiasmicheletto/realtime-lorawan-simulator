#ifndef NETWORKOPTIMIZERRANDOM_H
#define NETWORKOPTIMIZERRANDOM_H

#include "networkoptimizer.h"

class NetworkOptimizerRandom : public NetworkOptimizer {
    private:
        void step();
    public:
        NetworkOptimizerRandom();
};

#endif