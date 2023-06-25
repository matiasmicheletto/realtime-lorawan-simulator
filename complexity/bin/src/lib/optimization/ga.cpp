#include "ga.h"
#include "openGA.hpp"

// Globals
Instance* _l;
Objective* _o;

struct Chromosome {
	unsigned int **x;
    
	void print() const { 
        _o->printSolution(x);
	}
};

struct MiddleCost {
	float cost;	
};

typedef EA::Genetic<Chromosome,MiddleCost> GA_Type;
typedef EA::GenerationType<Chromosome,MiddleCost> Generation_Type;

void init_genes(Chromosome& p, const std::function<double(void)> &rnd01) {
	p.x = (unsigned int**) malloc( sizeof(unsigned int*) * _l->getEDCount());
    for(unsigned int i = 0; i < _l->getEDCount(); i++){
        p.x[i] = (unsigned int*) malloc( sizeof(unsigned int) * 2 );
        std::vector<unsigned int> gwList = _l->getGWList(i); // Valid gws for this ed
        p.x[i][VINDEX::GW] = gwList.at((unsigned int)floor(rnd01()*gwList.size())); // Pick random gw
        unsigned int minSF = 8, maxSF = 7;
        while(minSF > maxSF){
            minSF = _l->getMinSF(i, p.x[i][VINDEX::GW]);
            maxSF = _l->getMaxSF(_l->getPeriod(i));
        }
        p.x[i][VINDEX::SF] = (unsigned int)floor(rnd01()*(double)(maxSF - minSF) + (double)minSF);
    }
}

void copy_genes(const Chromosome& from, Chromosome& to) {
    for(unsigned int i = 0; i < _l->getEDCount(); i++){
        to.x[i][VINDEX::GW] = from.x[i][VINDEX::GW];
        to.x[i][VINDEX::SF] = from.x[i][VINDEX::SF];
    }
}

Chromosome mutate(const Chromosome& X_base, const std::function<double(void)> &rnd01, double shrink_scale) {
	Chromosome X_new;
    
    // TODO

	return X_new;
}

Chromosome crossover(const Chromosome& X1, const Chromosome& X2, const std::function<double(void)> &rnd01) {
	Chromosome X_new;
    init_genes(X_new, rnd01);
    
    // TODO

	return X_new;
}

bool eval_solution(const Chromosome& p, MiddleCost &c) { // Compute costs
    unsigned int gwCount, energy;
    double totalUF;
    c.cost = _o->eval(p.x, gwCount, energy, totalUF);
	return c.cost < __DBL_MAX__;
}

double calculate_SO_total_fitness(const GA_Type::thisChromosomeType &X) { // Compute fitness value
    return X.middle_costs.cost;    
}

void SO_report_generation(int generation_number, const EA::GenerationType<Chromosome,MiddleCost> &last_generation, const Chromosome& best_genes) {
    std::cout << "Generation [" << generation_number << "], "
		<< "Best total cost = " << last_generation.best_total_cost << ", "
		<< "Average cost = " << last_generation.average_cost << ", "
		<< "Best value :" << std::endl;
    
    best_genes.print();
        
    std::cout << "Best index = " << last_generation.best_chromosome_index << ", "
		<< "Exec. time = " << last_generation.exe_time
		<< std::endl;
}

void ga(Instance* l, Objective* o, const GAConfig& config) {

    _l = l;
    _o = o;

    std::cout << "GA optimization..." << std::endl;
    std::cout << "Pop. size: " << config.popsize << std::endl;
    std::cout << "Max. generations: " << config.maxgen << std::endl;

    EA::Chronometer timer;
	timer.tic();

	// Set GA configuration
	GA_Type ga_obj;
	ga_obj.problem_mode = EA::GA_MODE::SOGA;
	ga_obj.multi_threading = true;
	ga_obj.idle_delay_us = 1; // switch between threads quickly
	ga_obj.verbose = false;
	ga_obj.population = config.popsize;
	ga_obj.generation_max = config.maxgen;
	ga_obj.calculate_SO_total_fitness = calculate_SO_total_fitness;
	ga_obj.init_genes = init_genes;
	ga_obj.eval_solution = eval_solution;
	ga_obj.mutate = mutate;
	ga_obj.crossover = crossover;    
	ga_obj.SO_report_generation = SO_report_generation;
    //ga_obj.elite_count = 10;
	ga_obj.crossover_fraction = config.crossover;
	ga_obj.mutation_rate = config.mutation;
    
	// Start optimization
	ga_obj.solve();
}