#include "networkoptimizer.h"

using namespace std;
using namespace std::chrono;

NetworkOptimizer::NetworkOptimizer() {
    srand(time(NULL));
    this->lastID = 0;
    this->H = 1;
    for(int i = 0; i < 16; i++)
        this->availableChannels[i] = true;
}

NetworkOptimizer::~NetworkOptimizer() {
    for (unsigned int i = 0; i < this->gateways.size(); i++)
        delete this->gateways[i];
    for (unsigned int i = 0; i < this->enddevices.size(); i++) 
        delete this->enddevices[i];
    this->gateways.clear();
    this->enddevices.clear();
}

void NetworkOptimizer::init(
        unsigned int networkSize, 
        unsigned int mapSize, 
        Random *posGenerator,
        Random *periodGenerator
    ) {
    this->mapSize = mapSize;
    
    // Period values and probabilities for the end devices
    for (unsigned int i = 0; i < networkSize; i++) {
        // End device position
        double x, y;
        posGenerator->setRandom(x, y);
        // End device period
        unsigned int period = periodGenerator->randomInt();
        this->H = lcm(this->H, period);
        // Add end device to the network
        this->createEndDevice(x, y, period);
    }
}

void NetworkOptimizer::createEndDevice(double x, double y, unsigned int period) {
    //printf("Creating end device at (%f, %f) with period %d\n", x, y, period);
    EndDevice *ed = new EndDevice(x, y, this->lastID, period);
    enddevices.push_back(ed);
    this->lastID++;
}

bool NetworkOptimizer::removeEndDevice(EndDevice *ed) {
    for (unsigned int i = 0; i < this->enddevices.size(); i++) {
        if (this->enddevices[i] == ed) {
            this->enddevices.erase(this->enddevices.begin() + i);
            delete this->enddevices[i];
            return true;
        }
    }
    return false;
}

bool NetworkOptimizer::createGateway(double x, double y) {
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

bool NetworkOptimizer::removeGateway(Gateway *gw) {
    for (unsigned int i = 0; i < this->gateways.size(); i++) {
        if (this->gateways[i] == gw) {
            this->gateways.erase(this->gateways.begin() + i);
            delete this->gateways[i];
            return true;
        }
    }
    return false;
}

void NetworkOptimizer::autoConnect() {
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

void NetworkOptimizer::disconnect() {
    for(int i = 0; i < this->gateways.size(); i++)
        this->gateways[i]->disconnect();
}

void NetworkOptimizer::step() {
    // Override this function in the child class
}

double NetworkOptimizer::getEDCoverage() {
    double coverage = 0;
    for (int i = 0; i < this->enddevices.size(); i++)
        if (this->enddevices[i]->isConnected())
            coverage++;
    return coverage / (double) this->enddevices.size();
}

void NetworkOptimizer::minimizeGW(unsigned int iters,unsigned int timeout) {
    steady_clock::time_point begin = steady_clock::now();

    // Remove all gateways and left a single one
    for(int i = 0; i < this->gateways.size(); i++)
        this->removeGateway(this->gateways[i]);
    
    Uniform gwPosGenerator = Uniform(-(double)this->mapSize/2, (double)this->mapSize/2);
    double x, y; 
    gwPosGenerator.setRandom(x, y);
    this->createGateway(x, y);
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
        double coverage = this->getEDCoverage();
        printf("Iteration %d: Coverage %.2f\n", i, coverage);
        if(coverage > 0.9999)
            break;
        else{
            suboptimalSteps++;
            if(suboptimalSteps > 15){
                gwPosGenerator.setRandom(x, y);
                this->createGateway(x, y);
                this->autoConnect();
                suboptimalSteps = 0;
            }
        }
    }
}

void NetworkOptimizer::printStatus() {
    printf("Network status:\n");
    printf("  Map size: %d\n", this->mapSize);
    printf("  Gateways: %d\n", this->gateways.size());
    printf("  End devices: %d\n", this->enddevices.size());
    printf("  ED coverage: %f\%\n", this->getEDCoverage()*100);
    // Print gateways positions
    printf("Gateways positions:\n");
    for(int i = 0; i < this->gateways.size(); i++)
        printf("  #%d: (%f, %f)\n", this->gateways[i]->getId(), this->gateways[i]->getX(), this->gateways[i]->getY());
}