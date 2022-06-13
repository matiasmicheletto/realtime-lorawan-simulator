#include "loranetwork.h"

LoraNetwork::LoraNetwork() {
    this->lastID = 0;
    this->H = 1;
}

LoraNetwork::~LoraNetwork() {
    for (unsigned int i = 0; i < this->gateways.size(); i++)
        delete this->gateways[i];
    for (unsigned int i = 0; i < this->enddevices.size(); i++) 
        delete this->enddevices[i];
}

void LoraNetwork::init(
        unsigned int networkSize, 
        unsigned int mapSize, 
        ED_DIST posDist,
        unsigned int *periods,
        double *prob
    ) {
    this->mapSize = mapSize;
    
    // Period values and probabilities for the end devices
    for (unsigned int i = 0; i < networkSize; i++) {
        // End device position
        double x, y;
        if(posDist == UNIFORM) {
            x = uniformDist() * mapSize;
            y = uniformDist() * mapSize;
        }
        if(posDist == NORMAL) { // Some eds may fall outside the map
            x = normalDist() * mapSize;
            y = normalDist() * mapSize;
        }
        // End device period
        //const int period = randomSelect(periods, prob, 3);
        const int period = 3600; // TODO
        this->H = lcm(this->H, period);
        // Add end device to the network
        this->createEndDevice(x, y, period);
    }
}

void LoraNetwork::createEndDevice(double x, double y, unsigned int period) {
    EndDevice *ed = new EndDevice(x, y, this->lastID, period);
    enddevices.push_back(ed);
    this->lastID++;
}

bool LoraNetwork::removeEndDevice(EndDevice *ed) {
    for (unsigned int i = 0; i < this->enddevices.size(); i++) {
        if (this->enddevices[i] == ed) {
            this->enddevices.erase(this->enddevices.begin() + i);
            delete this->enddevices[i];
            return true;
        }
    }
    return false;
}

void LoraNetwork::createGateway(double x, double y) {
    Gateway *gw = new Gateway(x, y, this->lastID, this->H);
    gateways.push_back(gw);
    this->lastID++;
}

bool LoraNetwork::removeGateway(Gateway *gw) {
    for (unsigned int i = 0; i < this->gateways.size(); i++) {
        if (this->gateways[i] == gw) {
            this->gateways.erase(this->gateways.begin() + i);
            delete this->gateways[i];
            return true;
        }
    }
    return false;
}

void LoraNetwork::autoConnect() {

}

void LoraNetwork::step() {
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

void LoraNetwork::minimizeGW(unsigned int iters,unsigned int timeout) {
    // TODO
}

void LoraNetwork::printStatus() {
    printf("Network status:\n");
    printf("  Map size: %d\n", this->mapSize);
    printf("  Gateways: %d\n", this->gateways.size());
    printf("  End devices: %d\n", this->enddevices.size());
}