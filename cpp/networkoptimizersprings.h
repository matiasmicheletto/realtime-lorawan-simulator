#ifndef NETWORKOPTIMIZERSPRINGS_H
#define NETWORKOPTIMIZERSPRINGS_H

#include "networkoptimizer.h"

class NetworkOptimizerSprings : public NetworkOptimizer {
    private:
        void step();
    public:
        NetworkOptimizerSprings();
};

#endif