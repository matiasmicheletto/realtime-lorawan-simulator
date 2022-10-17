#include <string>
#include <iostream>
#include <fstream>
#include "openGA.hpp"

using namespace std;

unsigned int _enddevices = 1000;
unsigned int _alpha = 1000;
unsigned int _beta = 1;

struct Chromosome {
	bool* gwArr;

	string to_string() const { // Print phenotype ??
		return "{ed:"+std::to_string(_enddevices)+"}";
	}
};

struct MiddleCost {
	double cost_gw_num;
	double cost_coverage;
};

typedef EA::Genetic<Chromosome,MiddleCost> GA_Type;
typedef EA::GenerationType<Chromosome,MiddleCost> Generation_Type;

void init_genes(Chromosome& p,const function<double(void)> &rnd01) {
	p.gwArr = (bool*) malloc(sizeof(bool)*_enddevices);
	for(unsigned int i = 0; i < _enddevices; i++)
		p.gwArr[i] = rnd01() < 0.001; // 1 gw each 1000 eds
}

bool eval_solution(const Chromosome& p, MiddleCost &c) { // Compute costs
	c.cost_coverage = 0;
	c.cost_gw_num = 0;
	return true; // genes are accepted
}

Chromosome mutate(const Chromosome& X_base, const function<double(void)> &rnd01, double shrink_scale) {
	Chromosome X_new;
	// TODO
	return X_new;
}

Chromosome crossover(const Chromosome& X1, const Chromosome& X2,const function<double(void)> &rnd01) {
	Chromosome X_new;
	// TODO
	return X_new;
}

double calculate_SO_total_fitness(const GA_Type::thisChromosomeType &X) { // Compute fitness value
	return _alpha*X.middle_costs.cost_coverage + _beta*X.middle_costs.cost_gw_num;
}

void SO_report_generation(int generation_number, const EA::GenerationType<Chromosome,MiddleCost> &last_generation, const Chromosome& best_genes) {
	// TODO
}

void save_results(const GA_Type &ga_obj) {
	// TODO
}

void optimizeGA() {
	EA::Chronometer timer;
	timer.tic();

	// Set GA configuration
	GA_Type ga_obj;
	ga_obj.problem_mode = EA::GA_MODE::NSGA_III;
	ga_obj.multi_threading = true;
	ga_obj.idle_delay_us = 1; // switch between threads quickly
	ga_obj.verbose = false;
	ga_obj.population = 40;
	ga_obj.generation_max = 100;
	ga_obj.calculate_SO_total_fitness = calculate_SO_total_fitness;
	ga_obj.init_genes = init_genes;
	ga_obj.eval_solution = eval_solution;
	ga_obj.mutate = mutate;
	ga_obj.crossover = crossover;
	ga_obj.SO_report_generation = SO_report_generation;
	ga_obj.crossover_fraction = 0.7;
	ga_obj.mutation_rate = 0.4;

	// Start optimization
	ga_obj.solve();

	// Print results
	cout << "Solution computed in " << timer.toc() << " seconds."<<endl;
	save_results(ga_obj);
}

int main(int argc, char **argv) {
	optimizeGA();
	return 0;
}