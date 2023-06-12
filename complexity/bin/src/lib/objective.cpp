#include "objective.h"

Objective::Objective(Instance* instance) {
    this->instance = instance;

    // Tunning parameters
    this->params[ALPHA] = 1.0;
    this->params[BETA] = 1.0;
    this->params[GAMMA] = 1.0;
}

Objective::~Objective() {

}

double Objective::eval(unsigned int** x) {
    bool usedGW[this->instance->getGWCount()];
    for(unsigned int j = 0; j < this->instance->getGWCount(); j++)
        usedGW[j] = false;
    
    for(unsigned int i = 0; i < this->instance->getEDCount(); i++){ // For each ED
        
        // Determine allowed SFs
        unsigned int minSF = this->instance->getSF(i, x[i][GW]);
        unsigned int maxSF = this->instance->getMaxSF(this->instance->getPeriod(i));
        if(x[i][SF] > maxSF || x[i][SF] < minSF) // Check if using valid SF
            //cout << "ED: " << i << ", SF: " << x[i][SF] << ", period: " << period << ",  (" << minSF << "," << maxSF << ")" << endl;
            return 0.0;

        // Mark used GW 
        usedGW[x[i][GW]] = true;
    }

    // Count number of gw used
    unsigned int gwCount = 0;
    for(unsigned int j = 0; j < this->instance->getGWCount(); j++)
        if(usedGW[j]) gwCount++;

    // Compute energy cost
    double energy = 0.0;
    for(unsigned int i = 0; i < this->instance->getEDCount(); i++) // For each ED
        energy += pow(2, x[i][SF] - 7);

    cout << "Used GW: " << gwCount << ", energy:" << energy << endl;

    return this->params[ALPHA] * (double)gwCount + this->params[BETA] * (double)energy;
}

