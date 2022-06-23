#include "libs/network/network.h"

int main(int argc, char **argv) {

    Network network = Network::Builder()
        .setPositionGenerator(POS_DIST::UNIFORM)
        ->setPeriodGenerator(PERIOD_DIST::SOFT)
        ->setMapSize(4000)
        ->setNetworkSize(5000)
        ->build();

    network.createGateway(-1000,-1000);
    network.createGateway(0, -1000);
    network.createGateway(1000, -1000);
    network.createGateway(-1000, 0);
    network.createGateway(0, 0);
    network.createGateway(1000, 0);
    network.createGateway(-1000, 1000);
    network.createGateway(0, 1000);
    network.createGateway(1000, 1000);

    const int minGWChannels = network.configureGWChannels();

    printf("Minimum GW channels: %d\n", minGWChannels);

    return 0;   
}