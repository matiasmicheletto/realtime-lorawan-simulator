#include "libs/network/network.h"
#include "libs/optimizer/optimizer.h"
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
    STEP_METHOD alg          = (STEP_METHOD) atoi(argv[7]);

    // Create models
    Network network = Network::Builder()
        .setPositionGenerator((POS_DIST) posDist)
        ->setPeriodGenerator((PERIOD_DIST) periodDist)
        ->setMapSize(mapSize)
        ->setNetworkSize(networkSize)
        ->build();
    
    Optimizer optimizer = Optimizer::Builder()
        .setNetwork(&network)
        ->setMaxIter(maxIter)
        ->setTimeout(timeout)
        ->setStepMethod(alg)
        ->build();
        
    // Run optimization
    optimizer.run();

    // Print and export results
    optimizer.printResults();

    char outputfilename[100];
    char csvfilename[100];
    string filename = "";
    for(int i = 1; i < 8; i++) 
        filename += "_" + to_string(atoi(argv[i]));
    strcpy(outputfilename, ("output"+filename+".txt").c_str());
    strcpy(csvfilename, ("nodes"+filename+".csv").c_str());

    network.exportNodesCSV(csvfilename);
    optimizer.exportFullResults(outputfilename);
    optimizer.appendToLog("summary.csv");
    
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
        printf("  optalg: optimization algorithm. 0->springs, 1->random, 2->random preserve\n");
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
    if(atoi(argv[7]) < 0 || atoi(argv[7]) > 2) {
        printf("Algorithm code must be between 0 and 2\n");
        exit(0);
    }
}
