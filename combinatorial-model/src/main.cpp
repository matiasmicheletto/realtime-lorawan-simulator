#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <limits>
#include <random>
#include <functional>
#include <chrono>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include "random/random.h"
#include "random/uniform.h"
#include "random/normal.h"
#include "random/clouds.h"
#include "random/custom.h"
#include "openGA.hpp"

#define LINE_BUFFER_SIZE 256 // Max length for csv file line
#define FILE_COLS 4 // Columns of csv file (id, x, y, period)

using namespace std;
using namespace std::chrono;

// Global default parameters (editable on binary args)
char* _inputfilename;
char* _outputfilename;
// Network config
unsigned int _mapsize = 1000;    
unsigned int _enddevices = 1000;
unsigned char _posdist = 0;    
unsigned char _perioddist = 0;
unsigned char _sfmax = 12;
// Finish conditions
unsigned int _itermax = 1000;
unsigned int _timeout = 360000; // ms
// Obj. Fc. parameters
unsigned int _alpha = 1000; // Cost of coverage < 100%
unsigned int _beta = 1; // Cost by gw cnt
// Opt. alg. method
unsigned char _algo = 1; // 0 -> ga (not implemented), 1 -> random
// Random opt. tunning parameters
unsigned int _initialGW = 1;
unsigned int _increaseGWAfter = 50;
// GA tunning parameters
double _mut_prob = 1/(double) _enddevices; // Remember to update this on _enddevices change
//double _cross_prob = 0.5; // NOT USED


struct Node { // End devices
    unsigned int id;
    float x;
    float y;
    unsigned int period;
    unsigned char maxSF; // Computed from period
};

struct GW { // Gateways
    unsigned int id;
    float uf[6];
    unsigned int ch;
};

struct Chromosome {
	bool* gwArr;

	string to_string() const { // Print number of gw
        unsigned int gwCnt = 0;
        for(unsigned int i = 0; i < _enddevices; i++){
            if(gwArr[i]){
                gwCnt++;
            }
        }
		return "GW count: " + std::to_string(gwCnt);
	}
};

struct MiddleCost {
	unsigned int cost;	
};

vector<Node> *_network;
unsigned char** _minSFMatrix;
steady_clock::time_point _begin;
bool* _bestSol;
unsigned int _minGWs;
unsigned int _currentIter;
unsigned char _exitCond;
unsigned char _channels;
float _coverage;
float _gwAvgUf;

// Binary random generator
auto binRnd = bind(uniform_int_distribution<>(0,1), default_random_engine());
random_device rd;  //Will be used to obtain a seed for the random number engine
mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
const float _eps = numeric_limits<float>::epsilon();


// GA typedefs
typedef EA::Genetic<Chromosome,MiddleCost> GA_Type;
typedef EA::GenerationType<Chromosome,MiddleCost> Generation_Type;



void printHelp() {    
    printf("Usage: ./runnable [-h | --help] [-f | --file] [-o | --output] [-a | --alpha] [-b | --beta]\n");
    
    printf("Arguments:\n");
    printf("  -h, --help            prints this help and exits.\n");
    printf("  -f, --file            network configuration file.\n");        
    printf("  or use the following arguments:\n");
    printf("    -m, --mapsize         size of the map (in meters).\n");
    printf("    -e, --enddevices      number of end devices.\n");
    printf("    -x, --positions       position distribution. 0->uniform, 1->normal, 2->clouds.\n");
    printf("    -p, --periods         period distribution. 0->soft, 1->medium, 2->hard.\n");
    printf("  -o, --output          output file name.\n");
    printf("  -i, --iterations      max. iterations.\n");
    printf("  -t, --timeout         max. run time (seconds).\n");
    printf("  -g, --gateways        initial gw number.\n");
    printf("  -s, --sfmax           maximum spreading factor for gateways.\n");
    printf("  -a, --algorithm       gateway configuration algorithm: 0->ga, 1->random.\n");       
    
    printf("Default values:\n");
    printf("  -f, --file            input.csv\n");
    printf("  -o, --output          output.dat\n");
    printf("  -i, --iterations      1000\n");    
    printf("  -t, --timeout         360\n");        
    printf("  -g, --gateways        1\n");    
    printf("  -a, --alpha           100\n");    
    printf("  -b, --beta            1\n");    

    printf("Examples: \n");
    printf("\t./runnable -f input.csv -o output.dat -a 1000 -b 5\n");
    exit(1);
}

// Utils
inline int gcd(int a, int b) {return b == 0 ? a : gcd(b, a % b);}
inline int lcm(int a, int b){return a * b / gcd(a, b);}
inline unsigned long int getElapsed() {
    return (unsigned long int) duration_cast<milliseconds>(steady_clock::now() - _begin).count();
}

unsigned char getMaxSF(const unsigned int period) {
    /*
        Compute the maximum allowable spread factor for an end device given
        its period
    */
    if(period > 3200)
        return 12;
    else if(period > 1600)
        return 11;
    else if(period > 800)
        return 10;
    else if(period > 400)
        return 9;
    else if(period > 200)
        return 8;
    else if(period > 100)
        return 7;
    else // No SF for this period
        return 0;
}

float getRange(const unsigned char sf) {
    /*
        Max ranges for different spread factors
    */
    switch(sf){
        case 12:
            return 2000.0;
        case 11:
            return 1000.0;
        case 10:
            return 500.0;
        case 9:
            return 250.0;
        case 8:
            return 125.0;
        case 7:
            return 62.5;
        default: 
            return 0;
    }
}

string getPosDistName(unsigned char posDist) {
    switch(posDist){
        case 0:
            return "Uniform";
        case 1:
            return "Normal";
        case 2:
            return "Clouds";
        case 3:
            return "External";
        default:
            return "Unknown";
    }
}

string getPeriodDistName(unsigned char periodDist) {
    switch(periodDist){
        case 0:
            return "Soft";
        case 1:
            return "Medium";
        case 2:
            return "Hard";
        case 3: 
            return "External";
        default:
            return "Unknown";
    }
}

string getExitConditionName(unsigned char exitCond) {
    switch(exitCond){
        case 0: 
            return "Max. iterations";
        case 1:
            return "Max. coverage";
        case 2:
            return "Timeout";
        default:
            return "Unknown";
    }
}

bool checkFileExists(string filename){
    FILE *file = fopen(filename.c_str(), "r");
    bool exists = file != NULL;
    if(exists) fclose(file);
    return exists;
}

void loadInstance() { 
    /*
        This function parses input csv file and builds a vector of nodes.        
    */
    FILE *file = fopen(_inputfilename, "r");
    if (file == NULL) {
        printf("Error opening file csv file\n");        
        exit(1);
    }

    _network = new vector<Node>;
    char line[LINE_BUFFER_SIZE];
    while ( fgets(line, LINE_BUFFER_SIZE, file) != NULL ) {
        int col = 0;
        char* token;
        unsigned int id, period;
        unsigned char maxSF;
        float x, y;        
        for (token = strtok(line, ","); 
            token != NULL && col < FILE_COLS; 
            col++, token=strtok(NULL, ","))
        {
            switch (col){
                case 0: 
                    id = atoi(token);
                    break;
                case 1:
                    x = atof(token);
                    break;
                case 2:
                    y = atof(token);
                    break;
                case 3:
                    period = atoi(token);
                    maxSF = getMaxSF(period);                    
                    break;
                default:
                    break;
            }
        }
        _network->push_back({id, x, y, period, maxSF});
    }
    fclose(file);

    _enddevices = _network->size(); // Update global value

    printf("-----------------------------------\n");
    printf("Configuration file loaded\n");
    printf("End devices: %d\n", _enddevices);        
    printf("-----------------------------------\n\n");
}

void buildInstance() {
    Random* posGenerator;
    // Create the position generator function
    switch (_posdist) {
        case 0: // Positions have a uniform distribution
            posGenerator = new Uniform(-(float)_mapsize/2, (float)_mapsize/2);
            break;
        case 1: // Positions have normal distribution
            posGenerator = new Normal(-(float)_mapsize/2, (float)_mapsize/2);
            break;
        case 2:{ // Positions fall with a uniform distribution of 3 normal distributions 
            posGenerator = new Clouds(-(float)_mapsize/2, (float)_mapsize/2, 5);
            break;
        }
        default:
            printf("Warning: Invalid position distribution.\n");
            break;
    }
    CustomDist::Builder distBuilder = CustomDist::Builder();
    switch (_perioddist){
        case 0:
            distBuilder.addValue(16000, 0.25)
                ->addValue(8000, 0.25)
                ->addValue(4000, 0.25)
                ->addValue(3200, 0.25);

            break;
        case 1:
		    distBuilder.addValue(8000, 0.25)
                ->addValue(4000, 0.25)
                ->addValue(2000, 0.25)
                ->addValue(1600, 0.25);
            break;
        case 2:
            distBuilder.addValue(1600, 0.25)
                ->addValue(800, 0.25)
                ->addValue(400, 0.25)
                ->addValue(320, 0.25);
            break;
        default:
            printf("Warning: Invalid period distribution.\n");
            break;
        break;
    }
    CustomDist *periodGenerator = new CustomDist(distBuilder.build());

    _network = new vector<Node>;
    for(unsigned int i = 0; i < _enddevices; i++){
        float x, y; 
        posGenerator->setRandom(x, y);
        unsigned int period = periodGenerator->randomInt();
        unsigned char maxSF = getMaxSF(period);
        _network->push_back({i, x, y, period, maxSF});
    }

    delete posGenerator;
    delete periodGenerator;

    printf("-----------------------------------\n");
    printf("Network built\n");
    printf("  Map size: %d\n", _mapsize);
    printf("  End devices: %d\n", _enddevices);
    printf("  Max. SF: %d\n", _sfmax);
    printf("  ED pos. generator: %s\n", getPosDistName(_posdist).c_str());
    printf("  ED period generator: %s\n", getPeriodDistName(_perioddist).c_str());
    printf("-----------------------------------\n\n");
}

// Euclidean distance between nodes i and j (computationally expensive, use memoization)
float computeDistance(const unsigned int i, const unsigned int j) { 
    /*
        This function computes the euclidean distance between nodes i and j.
        i and j are the indexes of vector of nodes.
        It is assumed that:         
        0 <= i < N and 
        0 <= j < N.
    */
    float dx = _network->at(i).x - _network->at(j).x;
    float dy = _network->at(i).y - _network->at(j).y;
    return sqrt(dx*dx + dy*dy);
}

// Compute the min SF matrix for network nodes
void buildMinSFMatrix() {
    /*
        Builds the minimum SF matrix.
        matrix[i][j] means min SF for connecting nodes (i+1) and j.
        INPORTANT: This matrix is triangular, so i,j: i > j
    */
    _minSFMatrix = (unsigned char**) malloc(sizeof(unsigned char*)*_enddevices - 1);
    for(unsigned int i = 0; i < _enddevices - 1; i++){
        _minSFMatrix[i] = (unsigned char*) malloc(sizeof(unsigned char)*(i+1));
        for(unsigned int j = 0; j < i+1; j++){
            float distance = computeDistance(i+1, j);
            if(distance < 62.5)
                _minSFMatrix[i][j] = 7;
            else if(distance < 125)
                _minSFMatrix[i][j] = 8;
            else if(distance < 250)
                _minSFMatrix[i][j] = 9;
            else if(distance < 500)
                _minSFMatrix[i][j] = 10;
            else if(distance < 1000)
                _minSFMatrix[i][j] = 11;
            else if(distance < 2000)
                _minSFMatrix[i][j] = 12;
            else // No SF for this distance
                _minSFMatrix[i][j] = 13;
        }
    }
}

// Free pointers of min SF matrix
void destroyMinSFMatrix() {
    for(unsigned int i = 0; i < _enddevices - 1; i++)
        free(_minSFMatrix[i]);
    free(_minSFMatrix);
}

// Access min SF matrix of the network
unsigned char getMinSF(const unsigned int i, const unsigned int j) {    
    /*
        This function eases the acces to the min SF matrix.
        0 <= i < N and 
        0 <= j < N.
    */
    if(i == j) return 7; // If gw is on ed position, sf=7
    return i < j ? _minSFMatrix[j-1][i] : _minSFMatrix[i-1][j];
}

unsigned int objectiveFunction(const bool *sol, unsigned int &connectedNodes, const bool stats = false) {
    /*
        Given the array of nodes that are gw, assign a gw for
        each node and compute a quality value depending on the 
        coverage % and the number of gw.
        Algorithm:            
            for each sf from 7 to 12:
                for each node:
                    if not connected:
                        for each gw:
                            try to connect; (checking allowed sf and uf[sf] of gw)
    */
    // Compute number of gw    
    vector<GW> *gateways = new vector<GW>;
    bool connected[_enddevices];
    for(unsigned int i = 0; i < _enddevices; i++){
        connected[i] = false; // Initially disconected
        if(sol[i]) // If node has GW
            gateways->push_back({i, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, 0}); // Init gw empty UF and channel=0
    }
    connectedNodes = 0;

    if(gateways->size() == 0)
        return _alpha*_enddevices;

    for(unsigned char sf = 7; sf <= _sfmax; sf++){ // For each SF (7..12)
        for(unsigned int i = 0; i < _enddevices; i++){ // For each ED
            if(!connected[i]){ // If node is not connected
                for(unsigned int j = 0; j < gateways->size(); j++){ // For each gateway
                    // Check if feasible sf (before computing UF)
                    if(sf >= getMinSF(i, gateways->at(j).id) && sf <= _network->at(i).maxSF){ 
                        const float requiredUF = pow(2, sf-7) / ((float) _network->at(i).period - pow(2,sf-7));
                        if(gateways->at(j).uf[sf-7]+requiredUF < 1.0){ // If possible to connect
                            gateways->at(j).uf[sf-7] += requiredUF; // Compute UF for this GW and sf
                            connected[i] = true; // Mark node as connected
                            connectedNodes++; // Increment counter
                            break; // Do no continue with same gw
                        }
                    }
                }
            }
        }
    }

    // Compute objective cost
    const unsigned int cost = _alpha*(_enddevices - connectedNodes) + _beta*gateways->size();

    if(stats){ // Expensive block
        // Compute network coverage %
        _coverage = (float)connectedNodes / (float) _enddevices * 100.0;
        
        // Compute Avg. UF for gateways
        _gwAvgUf = 0.0;
        for(unsigned int i = 0; i < gateways->size(); i++){
            float ufAvg = 0.0;
            for(unsigned int j = 0; j < 6; j++){
                ufAvg += gateways->at(i).uf[j];
            }
            ufAvg /= 6.0;
            _gwAvgUf += ufAvg;
        }
        _gwAvgUf /= gateways->size();
        
        // Compute channels
        if(gateways->size() > 16){
            _channels = 0;
            list<unsigned int> usedChannels;
            for(unsigned int i = 1; i < gateways->size(); i++){ // Start from 1 to avoid GW 0
                // Get a list of channels used by the neighbors of the current gateway
                usedChannels.clear();
                for(unsigned int j = 0; j < gateways->size(); j++){
                    if(i == j) continue; // Skip current gateway
                    unsigned char minsfi = 7, minsfj = 7; // Min. used SF of each GW
                    for(unsigned char s = 7; s < 12; s++){
                        if(gateways->at(i).uf[s-7] > _eps) // SF is used
                            if(s > minsfi)
                                minsfi = s;
                        if(gateways->at(j).uf[s-7] > _eps) // SF is used
                            if(s > minsfj)
                                minsfj = s;
                    }
                    const float rangei = getRange(minsfi);
                    const float rangej = getRange(minsfj);
                    const float distance = computeDistance(gateways->at(i).id, gateways->at(j).id);
                    if(distance < rangei+rangej){ // i and j are neighbors
                        usedChannels.push_back(gateways->at(j).ch); // Add channel of j to the list
                    }
                }
                // Traverse the list of used channels and find the minimum available
                int minAvailableChannel = 0;
                for(unsigned int j = 0; j < gateways->size(); j++){ // Worst case is one channel per gateway
                    if(find(usedChannels.begin(), usedChannels.end(), j) == usedChannels.end()){ // if channel j not found
                        minAvailableChannel = j; // If j is not used, then is the minimum available channel
                        break; // Stop search
                    }
                }
                gateways->at(i).ch = minAvailableChannel;
                // Update minimum number of channels
                if(minAvailableChannel+1 > _channels)
                    _channels = minAvailableChannel+1;
            }  
        }else{ // If GW count < 16, assume 1 channel for each gw
            _channels = gateways->size();
        }
    }

    // Free used memory    
    gateways->clear();
    delete gateways;
    
    return cost;
}

void printSummary() {
    /*
        Print summarized results to summary.csv
    */

    bool printHeader = !checkFileExists("summary.csv");

    FILE *logfile = fopen("summary.csv", "a");
    if(logfile == NULL){
        printf("Error opening log file.\n");
        return;
    }

    if(printHeader)        
        fprintf(logfile, "GW Pos. Heuristic,ED Sched. Method,Position distr.,Periods distr.,Map size,ED Number,ED Dens. (1/m^2),Max. SF,GW Number,Channels used,ED Unfeas. Instances,Coverage %%,GW Avg. UF,Elapsed,Iterations,Exit condition\n");

    fprintf(logfile, "Random Comb.,None,%s,%s,%d,%d,%.2f,%d,%d,%d,0,%.2f,%.2f,%ld,%d,%s\n",         
        getPosDistName(_posdist).c_str(),
        getPeriodDistName(_perioddist).c_str(),
        _mapsize, 
        _enddevices, 
        (float) _enddevices / (float) _mapsize / (float) _mapsize,
        _sfmax,
        _minGWs,
        _channels,
        _coverage,
        _gwAvgUf,
        getElapsed(),
        _currentIter,
        getExitConditionName(_exitCond).c_str()
    );

    fclose(logfile);
}

void setRandomSol(bool *sol, unsigned int gwn) {
    /*
        Generates a random binary array with only N ones (at random positions)
    */
    uniform_int_distribution<> distrib(0, _enddevices-1);
    for(unsigned int i = 0; i < _enddevices; i++)
        sol[i] = false; // Set all to false
    for(unsigned int i = 0; i < gwn; i++){
        unsigned int pos = (unsigned int) distrib(gen);
        while(sol[pos]) // TODO: In case of "gwn" close to "_enddevices", this could take a while
            pos = (unsigned int) distrib(gen);
        sol[pos] = true;
    }
}

void copySol(bool *from, bool *to) {
    /*
        Copy values between two solution (binary) arrays.
    */
    for(unsigned int i = 0; i < _enddevices; i++)
        to[i] = from[i];
}

void optimizeRandomHeuristic() {
    /*
        Search optima generating random solutions, but having control
        of the number of GW
    */
    _exitCond = 0;
    unsigned int minCost = _alpha*_enddevices;
    _minGWs = _enddevices;
    bool sol[_enddevices];
    unsigned int gws = _initialGW; // Number of gw
    unsigned int cn = 0; // connected EDs (determined when evaluating objective fc).    
    for(_currentIter = 0; _currentIter < _itermax; _currentIter++){
        setRandomSol(sol, gws); // Full random generator
        unsigned int cost = objectiveFunction(sol, cn, false);        
        if(cost < minCost){
            minCost = cost;
            _minGWs = gws;
            copySol(sol, _bestSol);
            printf("New min. found %d gw (cost=%d) on iter %d (%d connected)\n", _minGWs, minCost, _currentIter, cn);
            if(cn == _enddevices){ // Once all connected
                //gws--; Reduce gw number
                printf("Max. coverage reached.\n");
                _exitCond = 1;
                break;
            }
        }
        if(_currentIter % _increaseGWAfter == 0){
            if(gws < _enddevices)
                gws++;            
        }
        if(getElapsed() > _timeout){
            printf("Timeout.\n");
            _exitCond = 2;
            break;
        }
    }
    
    printf("Min. GW num is %d (cost=%d)\n", _minGWs, minCost);    

    // Check number of channels and restart optimization if it is greater than 16
    objectiveFunction(_bestSol, cn, true); // Run fc. obj. computing channels and UF avg.
    if(_channels >= 16 && _sfmax > 7 && _exitCond != 2){
        _sfmax--;
        printf("Number of channels = %d. Reducing SF Max. to %d.\n", _channels, _sfmax);
        optimizeRandomHeuristic();
    }
}



//// GA OPERATORS ////

void init_genes(Chromosome& p, const function<double(void)> &rnd01) {
	p.gwArr = (bool*) malloc(sizeof(bool)*_enddevices);
	for(unsigned int i = 0; i < _enddevices; i++)
		p.gwArr[i] = rnd01() < 0.001; // 1 gw every 1000 eds
}

Chromosome mutate(const Chromosome& X_base, const function<double(void)> &rnd01, double shrink_scale) {
	Chromosome X_new;

    X_new = X_base;
    for(unsigned int i = 0; i < _enddevices; i++){
        if(rnd01() < _mut_prob){
            X_new.gwArr[i] = !X_new.gwArr[i]; // Switch gene
        }
    }

	return X_new;
}

Chromosome crossover(const Chromosome& X1, const Chromosome& X2,const function<double(void)> &rnd01) {
	Chromosome X_new = X1;
    unsigned int x_point = (unsigned int) (rnd01()*(double)_enddevices); // Crossover point
    for(unsigned int i = x_point; i < _enddevices; i++){
        X_new.gwArr[i] = X2.gwArr[i];
    }
	return X_new;
}

bool eval_solution(const Chromosome& p, MiddleCost &c) { // Compute costs
    unsigned int cn;
    c.cost = objectiveFunction(p.gwArr, cn, false);
    //printf("[eval_solution] Cost = %d, ED = %d, connected = %d\n", cost, _enddevices, cn);

	return true; // genes always accepted (unrestricted optimization)
}

double calculate_SO_total_fitness(const GA_Type::thisChromosomeType &X) { // Compute fitness value
    //const unsigned int max_cost = (_alpha+_beta)*_enddevices;
	//return (double) (max_cost - X.middle_costs.cost);
    return X.middle_costs.cost;    
}

void SO_report_generation(int generation_number, const EA::GenerationType<Chromosome,MiddleCost> &last_generation, const Chromosome& best_genes) {
    (void) last_generation; // ??

    cout<<"Generation ["<<generation_number<<"], "
		<<"Best = "<<last_generation.best_total_cost<<", "
		<<"Average = "<<last_generation.average_cost<<", "
		<<"Best genes = ("<<best_genes.to_string()<<")"<<", "
		<<"Exe_time = "<<last_generation.exe_time
		<<endl;
}

void optimizeGA() {
    /*
        Configure GA optimizer and run
    */

	EA::Chronometer timer;
	timer.tic();

	// Set GA configuration
	GA_Type ga_obj;
	ga_obj.problem_mode = EA::GA_MODE::SOGA;
	ga_obj.multi_threading = true;
	ga_obj.idle_delay_us = 1; // switch between threads quickly
	ga_obj.verbose = false;
	ga_obj.population = 30;
	ga_obj.generation_max = _itermax/25;
    ga_obj.best_stall_max = _itermax/100;
    ga_obj.average_stall_max = _itermax/100;
	ga_obj.calculate_SO_total_fitness = calculate_SO_total_fitness;
	ga_obj.init_genes = init_genes;
	ga_obj.eval_solution = eval_solution;
	ga_obj.mutate = mutate;
	ga_obj.crossover = crossover;    
	ga_obj.SO_report_generation = SO_report_generation;
    //ga_obj.elite_count = 5;
	ga_obj.crossover_fraction = 0.5;
	ga_obj.mutation_rate = 0.3;
    
	// Start optimization
	ga_obj.solve();

    printf("Last generation:\n");
    for(unsigned int i = 0; i < ga_obj.population; i++){
        const auto &X = ga_obj.last_generation.chromosomes[i];
        unsigned int cn = 0;
        unsigned int cost = objectiveFunction(X.genes.gwArr, cn, true);
        printf("Xi = %d, cost = %d, ch = %d, coverage = %.2f, uf avg = %.2f   - ",i, cost, _channels, _coverage, _gwAvgUf);
        printf(X.genes.to_string().c_str());
        printf("\n");
    }

	// Print results
	cout << "Solution computed in " << timer.toc() << " seconds."<<endl;
}



int main(int argc, char **argv) {

    default_random_engine generator( random_device{}() ); // Random seed

    bool buildNetwork = true; // If nodes file is not provided, then build the network

    // Arguments parsing
    for(int i = 0; i < argc; i++) {
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
            printHelp();
        }
        if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
            if(i + 1 < argc) {
                _inputfilename = argv[i + 1];    
                loadInstance();
                _posdist = 3; // External
                _perioddist = 3; // External
                buildNetwork = false;
            }else
                printHelp();
        }
        if(strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--mapsize") == 0){
            if(i+1 < argc)
                _mapsize = atoi(argv[i+1]);
            else
                printHelp();
        }
        if(strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--enddevices") == 0){
            if(i+1 < argc){
                _enddevices = atoi(argv[i+1]);
                _mut_prob = 1/(double) _enddevices;
            }else
                printHelp();
        }
        if(strcmp(argv[i], "-x") == 0 || strcmp(argv[i], "--positions") == 0){
            if(i+1 < argc)
                _posdist = atoi(argv[i+1]);
            else
                printHelp();
        }
        if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--periods") == 0){
            if(i+1 < argc)
                _perioddist = atoi(argv[i+1]);
            else
                printHelp();
        }
        if(strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if(i + 1 < argc) {
                _outputfilename = argv[i + 1];
            }else
                printHelp();
        }
        if(strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--iterations") == 0){
            if(i+1 < argc)
                _itermax = atoi(argv[i+1]);
            else
                printHelp();
        }
        if(strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--gateways") == 0){
            if(i+1 < argc)
                _initialGW = atoi(argv[i+1]);
            else
                printHelp();
        }
        if(strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--sfmax") == 0){
            if(i+1 < argc)
                _sfmax = atoi(argv[i+1]);
            else
                printHelp();
        }
        if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--timeout") == 0){
            if(i+1 < argc)
                _timeout = atoi(argv[i+1])*1000; // s -> ms
            else
                printHelp();
        }
        if(strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--algorithm") == 0){
            if(i+1 < argc)
                _algo = atoi(argv[i+1]);
            else
                printHelp();
        }
    }

    _begin = steady_clock::now();

    if(buildNetwork)
        buildInstance();

    buildMinSFMatrix();
    printf("Min. SF matrix built (t = %ld ms).\n", getElapsed());
    
    _bestSol = (bool*) malloc(sizeof(bool)*_enddevices);

    printf("--------------------------------\n");
    printf("Optimizer started:\n");        
    printf("  Max. SF: %d\n", _sfmax);
    printf("  Initial number of GW: %d\n", _initialGW);        
    printf("  Add gateway after: %d iterations\n", _increaseGWAfter);
    printf("  Max. iterations: %d\n", _itermax);
    printf("  Timeout: %d s\n", _timeout/1000);
    printf("--------------------------------\n\n");

    if(_algo == 0)
        optimizeGA();

    if(_algo == 1)
        optimizeRandomHeuristic();

    // Print elapsed time
    printf("Total elapsed time is %ld ms.\n", getElapsed());
    printSummary();
    
    // Free pointers    
    destroyMinSFMatrix(); 
    _network->clear();
    delete _network;
    free(_bestSol);

    return 0;
}

