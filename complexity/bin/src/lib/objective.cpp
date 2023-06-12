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

bool Objective::feasible(unsigned int** x) {
    const unsigned int ed = this->instance->getEDCount();
    for(unsigned int i = 0; i < ed; i++){ // For each ED
        // Determine allowed SFs
        unsigned int minSF = this->instance->getSF(i, x[i][GW]);
        unsigned int period = this->instance->getPeriod(i);
        unsigned int maxSF = this->instance->getMaxSF(period);
        if(x[i][SF] > maxSF || x[i][SF] < minSF){ // Check if using valid SF
            cout << "ED: " << i << ", SF: " << x[i][SF] << ", period: " << period << ",  (" << minSF << "," << maxSF << ")" << endl;
            return false;
        }
    }
    return true;
}

double Objective::eval(unsigned int** x) {
    if(!feasible(x))
        return 0.0;

    double energy = 0.0;
    const unsigned int ed = this->instance->getEDCount();
    for(unsigned int i = 0; i < ed; i++) // For each ED
        energy += pow(2, x[i][SF] - 7);

    return this->params[BETA] * energy;
}
