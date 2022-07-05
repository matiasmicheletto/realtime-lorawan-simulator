#ifndef JSINTERFACE_H
#define JSINTERFACE_H

#include <emscripten.h>
#include "../libs/random/uniform.h"
#include "../libs/random/custom.h"
#include "../libs/network/gateway.h"
#include "../libs/network/enddevice.h"
#include "../libs/network/network.h"
#include "../libs/optimizer/optimizer.h"

class JsInterface {
    public:
        JsInterface(
            unsigned int mapSize,
            unsigned int networkSize,
            unsigned char maxSF,
            unsigned char posDist,
            unsigned char periodDist,
            unsigned int maxIter,
            unsigned int timeout,
            unsigned char alg,
            unsigned int updatePeriod
        );
        ~JsInterface();
        
        void run();
        static void progressCallback(Network *network);
    private:
        Network *network;
        Optimizer *optimizer;
};

#endif // JSINTERFACE_H