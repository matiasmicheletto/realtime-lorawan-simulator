#include "loranetwork.h"

LoraNetwork::LoraNetwork(int networkSize, int mapSize) {
    this->mapSize = mapSize;

    // Period values and probabilities for the end devices
    const int periods[] = { 3600, 1800, 900 };
    const double prob[] = { 0.97, 0.03, 0.01 };
    for (int i = 0; i < networkSize; i++) {
        // End device position
        const double x = uniformDist() * mapSize;
        const double y = uniformDist() * mapSize;
        // End device period
        //const int period = randomSelect(periods, prob, 3);
        const int period = 3600; // TODO
        // Add end device to the network
        EndDevice *ed = new EndDevice(x, y, period);
        enddevices.push_back(ed);
    }
}

LoraNetwork::~LoraNetwork() {
    for (int i = 0; i < enddevices.size(); i++)
        delete enddevices[i];
}

void LoraNetwork::autoConnect() {

}

void LoraNetwork::step() {

}

void LoraNetwork::run(int iters, int timeout) {

    // Get list of non connected end devices
    /*
    for (int i = 0; i < enddevices.size(); i++) {
        if (!enddevices[i]->isConnected()){
            // Get closest gateway
            double minDist = 0;
            int minIndex = 0;
            for (int j = 0; j < gateways.size(); j++) {
                double dist = enddevices[i]->distanceTo(gateways[j]);
                if (j == 0 || dist < minDist) {
                    minDist = dist;
                    minIndex = j;
                }
            }
            // Add new force

        }
    }
    // Disconnect end devices
    // Update gateways positions according to forces
    // Reconnect network
    */
}

void LoraNetwork::print() {
    printf("LoraNetwork:\n");
}