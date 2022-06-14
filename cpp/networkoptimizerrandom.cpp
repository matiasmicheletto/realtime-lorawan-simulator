#include "networkoptimizerrandom.h"

NetworkOptimizerRandom::NetworkOptimizerRandom() : NetworkOptimizer() {

}

void NetworkOptimizerRandom::step() {
    double originalCoverage = this->getEDCoverage();
    // Save original positions
    vector<vector<double>> originalPositions;
    for(int i = 0; i < this->gateways.size(); i++){
        double x = this->gateways[i]->getX();
        double y = this->gateways[i]->getY();
        originalPositions.push_back(vector<double>{x, y});
    }
    this->disconnect(); 
    // Randomize positions
    for(int i = 0; i < this->gateways.size(); i++){
        double newX = uniformDist() * mapSize;
        double newY = uniformDist() * mapSize;
        this->gateways[i]->moveTo(newX, newY);
    }
    // Check new coverage
    this->autoConnect();
    double newCoverage = this->getEDCoverage();
    if(newCoverage < originalCoverage){ // If coverage did not improve
        this->disconnect();
        // Restore original positions
        for(int i = 0; i < this->gateways.size(); i++){
            double x = originalPositions[i][0];
            double y = originalPositions[i][1];
            this->gateways[i]->moveTo(x, y);
        }
        originalPositions.clear();
        this->autoConnect();
    }
}