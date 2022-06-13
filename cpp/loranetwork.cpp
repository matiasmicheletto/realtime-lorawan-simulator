#include "loranetwork.h"

using namespace std;
using namespace std::chrono;

LoraNetwork::LoraNetwork() {
    this->lastID = 0;
    this->H = 1;
    for(int i = 0; i < 16; i++)
        this->availableChannels[i] = true;
}

LoraNetwork::~LoraNetwork() {
    for (unsigned int i = 0; i < this->gateways.size(); i++)
        delete this->gateways[i];
    for (unsigned int i = 0; i < this->enddevices.size(); i++) 
        delete this->enddevices[i];
    this->gateways.clear();
    this->enddevices.clear();
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

bool LoraNetwork::createGateway(double x, double y) {
    for(unsigned char channel = 0; channel < 16; channel++){
        if(this->availableChannels[channel]){
            Gateway *gw = new Gateway(x, y, this->lastID, this->H, channel);
            gateways.push_back(gw);
            this->lastID++;
            this->availableChannels[channel] = false;
            return true;
        }
    }
    return false;
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
    for(int i = 0; i < this->enddevices.size(); i++) {
        // Sort gateways by distance
        sort(this->gateways.begin(), this->gateways.end(), [this, i](Gateway *a, Gateway *b) {
            return a->distanceTo(this->enddevices[i]) < b->distanceTo(this->enddevices[i]);
        });
        for(int j = 0; j < this->gateways.size(); j++)
            if(this->gateways[j]->addEndDevice(this->enddevices[i]))
                break;
    }
}

void LoraNetwork::disconnect() {
    for(int i = 0; i < this->gateways.size(); i++)
        this->gateways[i]->disconnect();
}

void LoraNetwork::step() {
    unsigned int nced = 0; // Number of disconnected end devices
    vector<vector<int>> forces; // Attraction forces for each gateway
    for (int i = 0; i < this->enddevices.size(); i++) {
        if (!this->enddevices[i]->isConnected()){
            nced++; // Count disconnected end device
            // Sort gateways by distance to get closest
            sort(this->gateways.begin(), this->gateways.end(), [this, i](Gateway *a, Gateway *b) {
                return a->distanceTo(this->enddevices[i]) < b->distanceTo(this->enddevices[i]);
            });
            // Compute attraction force for closest (this->gateways[0])
            int id = this->gateways[0]->getId();
            int x = (int)this->enddevices[i]->getX() - (int)this->gateways[0]->getX();
            int y = (int)this->enddevices[i]->getY() - (int)this->gateways[0]->getY();
            forces.push_back(vector<int>{id, x, y});
        }
    }
    // Disconnect all nodes
    this->disconnect();
    // Update gateways positions according to forces
    for(int i = 0; i < this->gateways.size(); i++){
        // Comoute sum of forces for current gateway
        double sumX = 0, sumY = 0; 
        for(int j = 0; j < forces.size(); j++){
            if(forces[j][0] == this->gateways[i]->getId()){
                sumX += (double) forces[j][1];
                sumY += (double) forces[j][2];
            }
        }
        // Compute new positions
        double newX = this->gateways[i]->getX() + clamp(sumX/(double) nced, -100.0, 100.0);
        double newY = this->gateways[i]->getY() + clamp(sumY/(double) nced, -100.0, 100.0);
        this->gateways[i]->moveTo(newX, newY);
    }
    // Reconnect network
    this->autoConnect();
}

double LoraNetwork::getEDCoverage() {
    double coverage = 0;
    for (int i = 0; i < this->enddevices.size(); i++)
        if (this->enddevices[i]->isConnected())
            coverage++;
    return coverage / (double) this->enddevices.size();
}

void LoraNetwork::minimizeGW(unsigned int iters,unsigned int timeout) {
    steady_clock::time_point begin = steady_clock::now();

    this->createGateway(uniformDist() * mapSize, uniformDist() * mapSize);
    this->autoConnect();
    unsigned int suboptimalSteps = 0;

    for(int i = 0; i < iters; i++){
        this->step();

        // Exit condition: timeout
        steady_clock::time_point end = steady_clock::now();
        double elapsed = duration_cast<seconds>(end - begin).count();
        if(elapsed > timeout)
            break;

        // Exit condition: max coverage reached
        if(this->getEDCoverage() == 1.0)
            break;
        else{
            suboptimalSteps++;
            if(suboptimalSteps > 10){
                this->createGateway(uniformDist() * mapSize, uniformDist() * mapSize);
                this->autoConnect();
                suboptimalSteps = 0;
            }
        }
    }
}

void LoraNetwork::printStatus() {
    printf("Network status:\n");
    printf("  Map size: %d\n", this->mapSize);
    printf("  Gateways: %d\n", this->gateways.size());
    printf("  End devices: %d\n", this->enddevices.size());
}