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

double Objective::eval(unsigned int** x) { // Check feasibility of x and compute objective
    double GWUF[this->instance->getGWCount()];
    for(unsigned int j = 0; j < this->instance->getGWCount(); j++)
        GWUF[j] = 0.0;
    
    for(unsigned int i = 0; i < this->instance->getEDCount(); i++){ // For each ED    
        
        unsigned int edPeriod = this->instance->getPeriod(i);
        // Check if feasible
        unsigned int minSF = this->instance->getSF(i, x[i][GW]);
        unsigned int maxSF = this->instance->getMaxSF(edPeriod);
        if(x[i][SF] > maxSF || x[i][SF] < minSF) // Check if using valid SF
            //std::cout << "ED: " << i << ", SF: " << x[i][SF] << ", period: " << period << ",  (" << minSF << "," << maxSF << ")" << std::endl;
            return 0.0;

        // Compute GW UF
        double pw = pow(2, x[i][SF]-7); 
        GWUF[x[i][GW]] += pw / ((double)edPeriod - pw);
        if(GWUF[x[i][GW]] > 1.0)
            //std::cout << "ED: " << i << ", GW: " << x[i][GW] << ", UF: " << GWUF[x[i][GW]] << std::endl;
            return 0.0;
    }

    // Count number of gw used
    unsigned int gwCount = 0;
    for(unsigned int j = 0; j < this->instance->getGWCount(); j++)
        if(GWUF[j] > UFTHRES) gwCount++;

    // Compute energy cost
    double energy = 0.0;
    for(unsigned int i = 0; i < this->instance->getEDCount(); i++) // For each ED
        energy += pow(2, x[i][SF] - 7);

    std::cout << "Used GW: " << gwCount << ", energy:" << energy << std::endl;

    return this->params[ALPHA] * (double)gwCount + this->params[BETA] * (double)energy;
}

