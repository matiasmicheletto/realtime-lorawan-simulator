#include "libs/network/network.h"
#include <string>
#include <cstring>
#include <stdio.h>

using namespace std;

void checkInputs(int argc, char *argv[]);

int main(int argc, char **argv) {
    checkInputs(argc, argv);

    // Input parameters
    unsigned int mapSize     = (unsigned int) atoi(argv[1]);
    unsigned int networkSize = (unsigned int) atoi(argv[2]);
    unsigned int maxIter     = (unsigned int) atoi(argv[3]);
    unsigned int timeout     = (unsigned int) atoi(argv[4]);
    POS_DIST posDist         = (POS_DIST) atoi(argv[5]);
    PERIOD_DIST periodDist   = (PERIOD_DIST) atoi(argv[6]);
    //STEP_METHOD alg          = (STEP_METHOD) atoi(argv[7]);

    // Create models
    Network network = Network::Builder()
        .setPositionGenerator((POS_DIST) posDist)
        ->setPeriodGenerator((PERIOD_DIST) periodDist)
        ->setMapSize(mapSize)
        ->setNetworkSize(networkSize)
        ->build();

    // Make grid    
    for(int row = 0; row < 9; row++)    
        for(int col = 0; col < 9; col++){
            if(row%2==0 && col%2!=0 || row%2!=0 && col%2==0)
                continue;
            else{ 
                double x = row*500-2000;
                double y = col*500-2000;
                network.createGateway(x, y);
            }
        }
        
    // Connect end devices
    network.autoConnect();

    // Print results
    network.exportNodesCSV("especial.csv");
    
    return 0;
}


void checkInputs(int argc, char *argv[]) {
    if (argc != 8) { // All arguments are mandatory
        printf("Usage: ./runnable mapsize enddevices iters timeout posdist perioddist optalg\n");
        printf("Where:\n");
        printf("  mapsize: size of the map (in meters)\n");
        printf("  enddevices: number of end devices\n");
        printf("  iters: number of iterations\n");
        printf("  timeout: timeout for the optimization\n");
        printf("  posdist: position distribution. 0->uniform, 1->normal, 2->clouds\n");
        printf("  perioddist: period distribution. 0->soft, 1->medium, 2->hard\n");
        printf("  optalg: optimization algorithm. 0->random, 1->springs\n");
        printf("Examples: \n");
        printf("\t./runnable 1000 6500 100 60 0 0 0\n");
        printf("\t./runnable 1000 800 250 120 1 0 1\n");
        exit(0);
    }
    if(atoi(argv[1]) < 0 || atoi(argv[1]) > 100000) {
        printf("Map size must be between 0 and 100000\n");
        exit(0);
    }
    if(atoi(argv[2]) < 10 || atoi(argv[2]) > 500000) {
        printf("Network size must be between 10 and 500000\n");
        exit(0);
    }
    if(atoi(argv[3]) < 1 || atoi(argv[3]) > 10000) {
        printf("Maximum number of iterations must be between 1 and 10000\n");
        exit(0);
    }
    if(atoi(argv[4]) < 1 || atoi(argv[4]) > 10000) {
        printf("Timeout must be between 1 and 10000 seconds\n");
        exit(0);
    }
    if(atoi(argv[5]) < 0 || atoi(argv[5]) > 2) {
        printf("Position distribution must be between 0 and 2\n");
        exit(0);
    }
    if(atoi(argv[6]) < 0 || atoi(argv[6]) > 2) {
        printf("Period distribution must be between 0 and 2\n");
        exit(0);
    }
    if(atoi(argv[7]) != 0 && atoi(argv[7]) != 1) {
        printf("Algorithm valid codes are 0 or 1\n");
        exit(0);
    }
}
