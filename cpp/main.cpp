#include <stdlib.h>
#include <time.h>
#include "loranetwork.h"


int main(int argc, char const *argv[]) {

    srand(time(NULL));

    LoraNetwork *network = new LoraNetwork(500, 1000);
    network->run(100, 10);
    network->print();
    delete network;
    return 0;
}