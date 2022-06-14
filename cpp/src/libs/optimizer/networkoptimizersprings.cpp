#include "networkoptimizersprings.h"

NetworkOptimizerSprings::NetworkOptimizerSprings() : NetworkOptimizer() {

}

void NetworkOptimizerSprings::step() {
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
    if(nced != 0){
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
            double newX = this->gateways[i]->getX() + mclamp(sumX/(double) nced, -100.0, 100.0);
            double newY = this->gateways[i]->getY() + mclamp(sumY/(double) nced, -100.0, 100.0);
            this->gateways[i]->moveTo(newX, newY);
        }
        // Reconnect network
        this->autoConnect();
    }
}