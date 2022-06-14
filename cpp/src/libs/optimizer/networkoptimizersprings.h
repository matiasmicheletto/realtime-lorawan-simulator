#ifndef NETWORKOPTIMIZERSPRINGS_H
#define NETWORKOPTIMIZERSPRINGS_H

#include "../tools.h"
#include "networkoptimizer.h"

class NetworkOptimizerSprings : public NetworkOptimizer {
    private:
        void step();
    public:
        NetworkOptimizerSprings();
};

#endif