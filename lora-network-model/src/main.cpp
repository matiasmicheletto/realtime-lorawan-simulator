#include "libs/network/network.h"
#include "libs/optimizer/optimizer.h"
#include <string>
#include <cstring>
#include <stdio.h>

using namespace std;

void printHelp() {    
    printf("Usage: ./runnable [-f | --file] [-m | --mapsize] [-e | --enddevices] [-x | --positions] [-p | --periods] [-s | --sfmax] [-i | --iterations] [-t | --timeout] [-a | --algorithm] [-v | --verbose]\n");
    
    printf("Arguments:\n");
    printf("  -f, --file            network configuration file.\n");
    printf("  or use the following arguments:\n");
    printf("    -m, --mapsize         size of the map (in meters).\n");
    printf("    -e, --enddevices      number of end devices.\n");
    printf("    -x, --positions       position distribution. 0->uniform, 1->normal, 2->clouds.\n");
    printf("    -p, --periods         period distribution. 0->soft, 1->medium, 2->hard.\n");
    printf("  -g, --gateways        initial number of gateways.\n");    
    printf("  -s, --sfmax           maximum spreading factor for gateways.\n");
    printf("  -a, --algorithm       gateway configuration algorithm: 0->springs, 1->random.\n");
    printf("  -i, --iterations      number of iterations.\n");
    printf("  -t, --timeout         timeout for the optimization.\n");
    printf("  -v, --verbose         verbose mode: prints complete outputs.\n");
    
    printf("Default values:\n");
    printf("  -m, --mapsize         1000\n");
    printf("  -e, --enddevices      10000\n");
    printf("  -x, --positions       0\n");
    printf("  -p, --periods         0\n");
    printf("  -g, --gateways        1\n");
    printf("  -s, --sfmax           12\n");
    printf("  -a, --algorithm       0\n");
    printf("  -i, --iterations      500\n");
    printf("  -t, --timeout         60\n");
    printf("  -v, --verbose         true\n");

    printf("Examples: \n");
    printf("\t./runnable -f network_nodes.csv -s 11 -i 100 -t 60  -a 0\n");
    printf("\t./runnable -m 1000 -e 6500 -x 0 -p 0 -s 11 -i 100 -t 60  -a 0 -v\n");
    printf("\t./runnable -m 1000 -e 800  -x 1 -p 0 -s 10 -i 250 -t 120 -a 1\n");
    exit(1);
}

int main(int argc, char **argv) {
    
    Network::Builder networkBuilder = Network::Builder();
    Optimizer::Builder optimizerBuilder = Optimizer::Builder();
    bool buildNetwork = true; // If nodes file is not provided, then build the network
    bool verbose = false; // If verbose mode is enabled, then print complete outputs

    Network *network;
    Optimizer* optimizer;

    for(int i = 0; i < argc; i++) {
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
            printHelp();
        }

        if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
            if(i + 1 < argc) {
                network = new Network(argv[i + 1]);
                buildNetwork = false;
            }
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
                optimizerBuilder.setInitialGW(atoi(argv[i+1]));
            else
                printHelp();
        }
        if(strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--iterations") == 0){
            if(i+1 < argc)
                optimizerBuilder.setMaxIter(atoi(argv[i+1]));
            else
                printHelp();
        }
        if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--timeout") == 0){
            if(i+1 < argc)
                optimizerBuilder.setTimeout(atoi(argv[i+1]));
            else
                printHelp();
        }
        if(strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--algorithm") == 0){
            if(i+1 < argc)
                optimizerBuilder.setStepMethod((STEP_METHOD) atoi(argv[i+1]));
            else
                printHelp();
        }
        if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0){
            verbose = true;
        }
    }

    if(buildNetwork)
        network = new Network(networkBuilder.build());

    optimizerBuilder.setNetwork(network);
    optimizer = new Optimizer(optimizerBuilder.build());
        
    // Run optimization
    optimizer->run();

    // Print and export results
    optimizer->printResults();

    if(verbose) {
        char outputfilename[100];
        char csvfilename[100];
        char schedfilename[100];
        string filename = to_string(networkBuilder.mapSize) + "_" + 
                        to_string(networkBuilder.enddevices.size()) + "_" +
                        to_string(networkBuilder.posDist) + "_" + 
                        to_string(networkBuilder.periodDist) + "_" + 
                        to_string(networkBuilder.maxSF) + "_" + 
                        to_string(optimizerBuilder.initialGW) + "_" + 
                        to_string(optimizerBuilder.maxIter) + "_" + 
                        to_string(optimizerBuilder.timeout) + "_" + 
                        to_string(optimizerBuilder.stepMethod);
        strcpy(outputfilename, ("output_"+filename+".txt").c_str());
        strcpy(csvfilename, ("nodes_"+filename+".csv").c_str());
        strcpy(schedfilename, ("sched_"+filename+".csv").c_str());
        network->exportNodesCSV(csvfilename);
        network->printScheduler(schedfilename);
        optimizer->exportFullResults(outputfilename);
    }
    
    optimizer->appendToLog("summary.csv");
    
    delete optimizer;
    delete network;

    return 0;
}

