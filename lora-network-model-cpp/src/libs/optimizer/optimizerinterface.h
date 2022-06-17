#ifndef OPTIMIZERINTERFACE_H
#define OPTIMIZERINTERFACE_H

#include <emscripten.h>

#include "../random/uniform.h"
#include "../random/custom.h"
#include "../network/gateway.h"
#include "../network/enddevice.h"
#include "networkoptimizer.h"

class OptimizerInterface {
    public:
        OptimizerInterface(
            unsigned int mapSize,
            unsigned int networkSize,
            unsigned int maxIter,
            unsigned int timeout,
            unsigned char alg,
            int updatePeriod
        );
        ~OptimizerInterface();
        
        void run();
    
    private:
        NetworkOptimizer *opt;
};

#endif // OPTIMIZERINTERFACE_H