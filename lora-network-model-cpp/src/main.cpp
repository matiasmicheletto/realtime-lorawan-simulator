#include "libs/network/network.h"

using namespace std;

int main(int argc, char **argv) {
    // Create models

    Network network = Network(argv[1]);

    network.createGateway(463-500,273-500);
    network.createGateway(352-500,554-500);
    network.createGateway(219-500,564-500);
    network.createGateway(238-500,141-500);
    network.createGateway(922-500,949-500);
    network.createGateway(29-500,821-500);
    network.createGateway(466-500,182-500);

    network.autoConnect();

    network.exportNodesCSV("especial.csv");
    
    FILE *file = fopen("especial.txt", "w");
    network.printNetworkStatus(file);
    fclose(file);
    
    return 0;
}