#include "libs/network/network.h"
#include "libs/optimizer/optimizer.h"
#include <string>
#include <cstring>
#include <stdio.h>

using namespace std;

void printHelp() {    
    printf("Usage: ./runnable [-f | --file] [-m | --mapsize] [-e | --enddevices] [-x | --positions] [-p | --periods] [-s | --sfmax] [-o | --output]\n");
    
    printf("Arguments:\n");
    printf("  -f, --file            network configuration file.\n");
    printf("  or use the following arguments to build and export network:\n");
    printf("    -m, --mapsize         size of the map (in meters).\n");
    printf("    -e, --enddevices      number of end devices.\n");
    printf("    -x, --positions       position distribution. 0->uniform, 1->normal, 2->clouds.\n");
    printf("    -p, --periods         period distribution. 0->soft, 1->medium, 2->hard.\n");
    printf("  -g, --gateways        number of gateways for grid.\n");    
    printf("  -s, --sfmax           maximum spreading factor for gateways.\n");    
    printf("  -o, --output          output file name for exporting cplex input file.\n");
    
    printf("Default values:\n");
    printf("  -m, --mapsize         1000\n");
    printf("  -e, --enddevices      10000\n");
    printf("  -x, --positions       0\n");
    printf("  -p, --periods         0\n");
    printf("  -g, --gateways        9\n");
    printf("  -s, --sfmax           12\n");

    printf("Examples: \n");
    printf("\t./runnable -f network_nodes.csv -o instance.dat -g 100\n");    
    exit(1);
}

int main(int argc, char **argv) {
    
    Network::Builder networkBuilder = Network::Builder();
    bool buildNetwork = true; // If nodes file is not provided, then build the network    

    Network *network;    
    char* outputfilename;

    for(int i = 0; i < argc; i++) {
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
            printHelp();
        }
        if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
            if(i + 1 < argc) {
                network = new Network(argv[i + 1]);
                buildNetwork = false;
            }else
                printHelp();
        }
        if(strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if(i + 1 < argc) {
                outputfilename = argv[i + 1];
            }else
                printHelp();
        }
        if(strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--mapsize") == 0){
            if(i+1 < argc)
                networkBuilder.setMapSize(atoi(argv[i+1]));
            else
                printHelp();
        }
        if(strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--enddevices") == 0){
            if(i+1 < argc)
                networkBuilder.setNetworkSize(atoi(argv[i+1]));
            else
                printHelp();
        }
        if(strcmp(argv[i], "-x") == 0 || strcmp(argv[i], "--positions") == 0){
            if(i+1 < argc)
                networkBuilder.setPositionGenerator((POS_DIST) atoi(argv[i+1]));
            else
                printHelp();
        }
        if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--periods") == 0){
            if(i+1 < argc)
                networkBuilder.setPeriodGenerator((PERIOD_DIST) atoi(argv[i+1]));
            else
                printHelp();
        }
        if(strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--sfmax") == 0){
            if(i+1 < argc)
                networkBuilder.setMaxSF(atoi(argv[i+1]));
            else
                printHelp();
        }
        if(strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--gateways") == 0){
            if(i+1 < argc)
                networkBuilder.setGWCnt(atoi(argv[i+1]));
            else
                printHelp();
        }
    }

    if(buildNetwork)
        network = new Network(networkBuilder.build());

    //network->exportGWGrid(outputfilename);
    network->exportEDsCSV(outputfilename);

    delete network;

    return 0;
}

