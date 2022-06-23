#include "libs/network/network.h"

int main(int argc, char **argv) {
    
    // Load ED positions and periods from file
    Network network = Network(argv[1]);

    // Create the gateways
    network.createGateway(463-500,273-500);
    
    // Compute coverage
    network.autoConnect();

    // Export results
    network.exportNodesCSV("especial.csv");
    FILE *file = fopen("especial.txt", "w");
    network.printNetworkStatus(file);
    fclose(file);
    
    return 0;
}