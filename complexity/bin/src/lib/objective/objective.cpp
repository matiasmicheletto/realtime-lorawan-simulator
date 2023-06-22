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

double Objective::eval(unsigned int** x, unsigned int &gwCount, double &energy, double &totalUF) {
    double GWUF[this->instance->getGWCount()];
    for(unsigned int j = 0; j < this->instance->getGWCount(); j++)
        GWUF[j] = 0.0;
    gwCount = 0;
    energy = 0.0;
    totalUF = 0.0;

    for(unsigned int i = 0; i < this->instance->getEDCount(); i++){ // For each ED    
        unsigned int edPeriod = this->instance->getPeriod(i);
        
        // Check if feasible SF
        unsigned int minSF = this->instance->getMinSF(i, x[i][GW]);
        unsigned int maxSF = this->instance->getMaxSF(edPeriod);
        if(x[i][SF] > maxSF || x[i][SF] < minSF) // Unfeasibility condition 1
            //std::cout << "ED: " << i << ", SF: " << x[i][SF] << ", period: " << period << ",  (" << minSF << "," << maxSF << ")" << std::endl;
            return __DBL_MAX__;

        // Compute GW UF
        double pw = pow(2, x[i][SF]-7); 
        GWUF[x[i][GW]] += pw / ((double)edPeriod - pw);
        totalUF += GWUF[x[i][GW]];
        if(GWUF[x[i][GW]] > 1.0) // Unfeasibility condition 2
            //std::cout << "ED: " << i << ", GW: " << x[i][GW] << ", UF: " << GWUF[x[i][GW]] << std::endl;
            return __DBL_MAX__;
    }

    // Count number of gw used
    for(unsigned int j = 0; j < this->instance->getGWCount(); j++)
        if(GWUF[j] > UFTHRES) gwCount++;

    // Compute energy cost
    for(unsigned int i = 0; i < this->instance->getEDCount(); i++) // For each ED
        energy += pow(2, x[i][SF] - 7);


    return // Fc obj
        this->params[ALPHA] * (double)gwCount + 
        this->params[BETA] * energy + 
        this->params[GAMMA] * totalUF;
}

void Objective::printSolution(unsigned int** x, bool highlight){
    unsigned int gwCount;
    double energy;
    double totalUF;

    const double result = this->eval(x, gwCount, energy, totalUF);
    

    if(highlight) std::cout << "\033[1;31m";

    std::cout << "Objective: " << result << std::endl;
    std::cout   << "Used GW: " << gwCount 
                << ", Energy: " << energy 
                << ", Total UF: " << totalUF
                << std::endl;
    std::cout << "GW allocation for EDs GW[SF]:" << std::endl;
    for(unsigned int i = 0; i < this->instance->getEDCount(); i++) // For each ED    
        std::cout << x[i][GW] << "[" << x[i][SF] << "]\t";
    std::cout << std::endl;
    
    if(highlight) std::cout << "\033[0m\n";
}
