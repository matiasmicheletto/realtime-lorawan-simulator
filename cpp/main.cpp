#include <stdlib.h>
#include <time.h>
#include "loranetwork.h"


int main(int argc, char const *argv[]) {

    srand(time(NULL));

    unsigned int periods[] = { 3600, 1800, 900 };
    double prob[] = { 0.97, 0.03, 0.01 };

    LoraNetwork *network = new LoraNetwork();
    network->init(100, 1000, LoraNetwork::UNIFORM, periods, prob);
    
    network->minimizeGW(100, 10);
    network->printStatus();
    
    delete network;
    return 0;
}