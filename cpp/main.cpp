#include <stdlib.h>
#include <time.h>
#include "networkoptimizerrandom.h"
#include "networkoptimizersprings.h"

int main(int argc, char **argv) {
    srand(time(NULL));

    unsigned int periods[] = { 3600, 1800, 900 };
    double prob[] = { 0.97, 0.02, 0.01 };

    //NetworkOptimizerRandom *network = new NetworkOptimizerRandom();
    NetworkOptimizerSprings *network = new NetworkOptimizerSprings();
    network->init(6500, 1000, NetworkOptimizer::UNIFORM, periods, prob, 3);
    
    network->minimizeGW(100, 30);
    network->printStatus();
    
    delete network;
   
    return 0;
}