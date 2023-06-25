#ifndef GA_H
#define GA_H

#include "../instance/instance.h"
#include "../objective/objective.h"


struct GAConfig {
    int popsize;
    int maxgen;
    double crossover;
    double mutation;
    
    GAConfig(
            int popSize = 100, 
            int numGens = 100, 
            double crossRate = 0.8, 
            double mutRate = 0.1)
        : 
            popsize(popSize), 
            maxgen(numGens), 
            crossover(crossRate), 
            mutation(mutRate) {}
};

void ga(Instance* l, Objective* o, const GAConfig& config = GAConfig());

#endif // GA_H