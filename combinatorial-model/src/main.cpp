#include <algorithm>
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
#include <unistd.h>

#include "random/random.h"
#include "random/uniform.h"
#include "random/normal.h"
#include "random/clouds.h"
#include "random/custom.h"
#include "openGA.hpp"

#define LINE_BUFFER_SIZE 256 // Max length for csv file line
#define FILE_COLS 4 // Columns of csv file (id, x, y, period)

#define MAX_CHANNELS 16 // Max allowed number of channels for gws

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
unsigned char _algo = 1; // 0 -> ga, 1 -> random
// Random opt. tunning parameters
unsigned int _initialGW = 1;
unsigned int _increaseGWAfter = 50;
// GA tunning parameters
double _mut_prob = 1/(double) _enddevices; // Remember to update this on _enddevices change
double _mut_rate = 0.3; // Mutation rate
double _cross_frac = 0.5; // Crossover fraction
unsigned int _pop_size = 30; // Population size


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
	bool* isGW;

	string to_string() const { // Print number of gw
        unsigned int gwCnt = 0;
        for(unsigned int i = 0; i < _enddevices; i++){
            if(isGW[i]){
                gwCnt++;
            }
        }
		return std::to_string(gwCnt)+" GWs";
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

// Greedy
vector<vector<int>> _adj_matrix;

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
    printf("  -a, --algorithm       gateway configuration algorithm: 0->ga, 1->random, 2->greedy, 3->springs.\n");       
    printf("  -pop                  GA population size.\n");
    printf("  -cros                 GA crossover fraction (between 0 and 1).\n");
    printf("  -mut                  GA mutation rate (between 0 and 1).\n");
    
    printf("Default values:\n");
    printf("  -f, --file            input.csv\n");
    printf("  -o, --output          output.dat\n");
    printf("  -i, --iterations      1000\n");    
    printf("  -t, --timeout         360\n");        
    printf("  -g, --gateways        1\n");    
    printf("  -s, --sfmax           12\n");    
    printf("  -a, --algorithm       1 (random)\n");    
    printf("  -pop                  30\n");
    printf("  -cros                 0.5\n");
    printf("  -mut                  0.3\n");

    printf("Examples: \n");
    printf("\t./runnable -f input.csv -o output.dat\n");
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

string getAlgorithmName(unsigned char algo) {
    switch (algo)
    {
    case 0:
        return "GA Opt.";
    case 1:
        return "Random Comb.";
    case 2:
        return "Greedy";
    case 3:
        return "Springs";
    default:
        return "Unknown";
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

void computeDirectionVector(const unsigned int i, const unsigned int j, float &dx, float&dy){
    /*
        Difference of positions between nodes i and j
     */
    dx = _network->at(i).x - _network->at(j).x;
    dy = _network->at(i).y - _network->at(j).y;
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
    float dx, dy;
    computeDirectionVector(i, j, dx, dy);
    return sqrt(dx*dx + dy*dy);
}

unsigned int getClosestId(float x, float y){
    /* 
        Given x, y coordinates in the map, find the closest node
     */
    float minDist = 2*(float)_mapsize;
    unsigned int closest = _network->size()+1;
    for(unsigned int i = 0; i < _network->size(); i++){
        const float dx = x - _network->at(i).x;
        const float dy = y - _network->at(i).y;
        const float dist = sqrt(dx*dx + dy*dy);
        if(dist < minDist){
            minDist = dist;
            closest = i;
        }
    }
    return closest;
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

void moFunction(const bool *sol, bool *connected, unsigned int &connectedCount, unsigned int &gwCount, const bool stats) {
    /*
        Given the array of nodes that are GW, assign a GW for
        each node and compute the number of not connected nodes
        the number of gw (MO optimization).
        Algorithm:            
            for each sf from 7 to 12:
                for each node:
                    if not connected:
                        for each gw:
                            try to connect; (checking allowed sf and uf[sf] of gw)
    */
    // Compute number of gw    
    vector<GW> *gateways = new vector<GW>;
    for(unsigned int i = 0; i < _enddevices; i++){
        connected[i] = false; // Initially disconected
        if(sol[i]) // If node has GW
            gateways->push_back({i, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, 0}); // Init gw empty UF and channel=0
    }

    connectedCount = 0;
    gwCount = gateways->size();

    if(gwCount == 0){
        if(stats){
            _coverage = 0.0;
            _gwAvgUf = 0.0;
            _channels = 0.0;
        }
        return;
    }

    for(unsigned char sf = 7; sf <= _sfmax; sf++){ // For each SF (7..12)
        for(unsigned int i = 0; i < _enddevices; i++){ // For each ED
            if(!connected[i]){ // If node is not connected                
                for(unsigned int j = 0; j < gwCount; j++){ // For each gateway
                    //cout << "SF=" << sf << ", node=" << i << " gw=" << j << endl;
                    // Check if feasible sf (before computing UF)
                    if(sf >= getMinSF(i, gateways->at(j).id) && sf <= _network->at(i).maxSF){
                        const float pw = pow(2, sf-7);
                        const float requiredUF = pw / ((float) _network->at(i).period - pw);
                        if(gateways->at(j).uf[sf-7]+requiredUF < 1.0){ // If possible to connect
                            gateways->at(j).uf[sf-7] += requiredUF; // Compute UF for this GW and sf
                            connected[i] = true; // Mark node as connected
                            connectedCount++; // Increment counter
                            break; // Do no continue with same gw
                        }
                    }
                }
            }
        }
    }

    if(stats){ // Expensive block
        // Compute network coverage %
        _coverage = (float)connectedCount / (float) _enddevices * 100.0;
        
        // Compute Avg. UF for gateways
        _gwAvgUf = 0.0;
        for(unsigned int i = 0; i < gwCount; i++){
            float ufAvg = 0.0;
            for(unsigned int j = 0; j < 6; j++){
                ufAvg += gateways->at(i).uf[j];
            }
            ufAvg /= 6.0;
            _gwAvgUf += ufAvg;
        }
        _gwAvgUf /= gwCount;
        
        // Compute channels
        if(gwCount > MAX_CHANNELS){
            _channels = 0;
            list<unsigned int> usedChannels;
            for(unsigned int i = 1; i < gwCount; i++){ // Start from 1 to avoid GW 0
                // Get a list of channels used by the neighbors of the current gateway
                usedChannels.clear();
                for(unsigned int j = 0; j < gwCount; j++){
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
                for(unsigned int j = 0; j < gwCount; j++){ // Worst case is one channel per gateway
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
            _channels = gwCount;
        }
    }

    // Free used memory    
    gateways->clear();
    delete gateways;
}

unsigned int evalCost(unsigned int &connectedCount, unsigned int &gwCount) {
    return _alpha * (_enddevices - connectedCount) + _beta * gwCount;
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

    fprintf(logfile, "%s,None,%s,%s,%d,%d,%.2f,%d,%d,%d,0,%.2f,%.2f,%ld,%d,%s\n",         
        getAlgorithmName(_algo).c_str(),
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
    uniform_int_distribution<> distrib(0, _enddevices-1); // Avoid declare each time ?
    for(unsigned int i = 0; i < _enddevices; i++)
        sol[i] = false; // Set all to false
    for(unsigned int i = 0; i < gwn; i++){
        unsigned int pos = (unsigned int) distrib(gen);
        while(sol[pos]) // TODO: In case of "gwn" close to "_enddevices", this could take a while
            pos = (unsigned int) distrib(gen);
        sol[pos] = true;
    }
}

void addRandomGW(bool *sol) { 
    /*
        Adds a new GW (switches 0 to 1) in a random position
    */
    uniform_int_distribution<> distrib(0, _enddevices-1); // Avoid declare each time ?
    bool added = false;
    while(!added){
        unsigned int pos = (unsigned int) distrib(gen);
        if(!sol[pos]){ // If node is not GW
            sol[pos] = true; // Add GW
            added = true; // Break
        }
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
    unsigned int gwsCnt = 0; // Number of gw (computed, but it should be gwsCnt == gws)
    unsigned int cn = 0; // connected EDs (determined when evaluating objective fc).    
    bool connected[_enddevices]; // List of connected ed (used inside moFunction)
    for(_currentIter = 0; _currentIter < _itermax; _currentIter++){
        setRandomSol(sol, gws); // Full random generator
        moFunction(sol, connected, cn, gwsCnt, false);        
        const unsigned int cost = evalCost(cn, gwsCnt);
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
    _currentIter = _itermax;
    
    printf("Min. GW num is %d (cost=%d)\n", _minGWs, minCost);    

    // Check number of channels and restart optimization if it is greater than 16
    moFunction(_bestSol, connected, cn, _minGWs, true); // Run fc. obj. computing channels and UF avg.
    if(_channels >= MAX_CHANNELS && _sfmax > 7 && _exitCond != 2){
        _sfmax--;
        printf("Number of channels = %d. Reducing SF Max. to %d.\n", _channels, _sfmax);
        optimizeRandomHeuristic();
    }
}



/////////// GA OPT ///////////

void init_genes(Chromosome& p, const function<double(void)> &rnd01) {
	p.isGW = (bool*) malloc(sizeof(bool)*_enddevices);
    double pr = (double)_initialGW / (double)_enddevices; // Probability of gw
	for(unsigned int i = 0; i < _enddevices; i++)
		p.isGW[i] = rnd01() < pr; // 
}

void copy_genes(const Chromosome& from, Chromosome& to) {
    for(unsigned int i = 0; i < _enddevices; i++)
        to.isGW[i] = from.isGW[i];
}

Chromosome mutate(const Chromosome& X_base, const function<double(void)> &rnd01, double shrink_scale) {
	Chromosome X_new;
    init_genes(X_new, rnd01);
    copy_genes(X_base, X_new);

    for(unsigned int i = 0; i < _enddevices; i++){
        if(rnd01() < _mut_prob){
            X_new.isGW[i] = !X_new.isGW[i]; // Switch gene
        }
    }

	return X_new;
}

Chromosome crossover(const Chromosome& X1, const Chromosome& X2,const function<double(void)> &rnd01) {
	Chromosome X_new;
    init_genes(X_new, rnd01);
    unsigned int x_point = (unsigned int) (rnd01()*(double)_enddevices); // Crossover point
    for(unsigned int i = 0; i < x_point; i++){
        X_new.isGW[i] = X1.isGW[i];
    }
    for(unsigned int i = x_point; i < _enddevices; i++){
        X_new.isGW[i] = X2.isGW[i];
    }
	return X_new;
}

bool eval_solution(const Chromosome& p, MiddleCost &c) { // Compute costs
    unsigned int cn, gwCnt;
    bool connected[_enddevices];
    moFunction(p.isGW, connected, cn, gwCnt, false);
    c.cost = evalCost(cn, gwCnt);
    //printf("[eval_solution] Cost = %d, ED = %d, connected = %d\n", cost, _enddevices, cn);

	return true; // genes always accepted (unrestricted optimization)
}

double calculate_SO_total_fitness(const GA_Type::thisChromosomeType &X) { // Compute fitness value
    //const unsigned int max_cost = (_alpha+_beta)*_enddevices;
	//return (double) (max_cost - X.middle_costs.cost);
    return X.middle_costs.cost;    
}

void SO_report_generation(int generation_number, const EA::GenerationType<Chromosome,MiddleCost> &last_generation, const Chromosome& best_genes) {
    cout<<"Generation ["<<generation_number<<"], "
		<<"Best total cost = "<<last_generation.best_total_cost<<", "
		<<"Average cost = "<<last_generation.average_cost<<", "
		<<"Best value = "<<best_genes.to_string()<<", "
        //<<"Best index = "<<last_generation.best_chromosome_index<<", "
		//<<"Exe_time = "<<last_generation.exe_time
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
	ga_obj.population = _pop_size;
	ga_obj.generation_max = _itermax/ga_obj.population;
    ga_obj.best_stall_max = _itermax/100;
    ga_obj.average_stall_max = _itermax/100;
	ga_obj.calculate_SO_total_fitness = calculate_SO_total_fitness;
	ga_obj.init_genes = init_genes;
	ga_obj.eval_solution = eval_solution;
	ga_obj.mutate = mutate;
	ga_obj.crossover = crossover;    
	ga_obj.SO_report_generation = SO_report_generation;
    //ga_obj.elite_count = 10;
	ga_obj.crossover_fraction = _cross_frac;
	ga_obj.mutation_rate = _mut_rate;
    
	// Start optimization
	ga_obj.solve();

    /*
    printf("Last generation:\n");
    for(unsigned int i = 0; i < ga_obj.population; i++){
        const auto &X = ga_obj.last_generation.chromosomes[i];
        unsigned int cn, gwCnt;
        moFunction(X.genes.isGW, cn, gwCnt, true);
        unsigned int cost = evalCost(cn, gwCnt);
        printf("Xi = %d, cost = %d, ch = %d, coverage = %.2f, uf avg = %.2f   - ",i, cost, _channels, _coverage, _gwAvgUf);
        printf(X.genes.to_string().c_str());
        printf("\n");
    }
    */

	// Print results
	cout << "Solution computed in " << timer.toc() << " seconds."<<endl;

    // Check number of channels and restart optimization if it is greater than 16
    unsigned int cn;
    bool connected[_enddevices];
    moFunction(ga_obj.last_generation.chromosomes[ga_obj.last_generation.best_chromosome_index].genes.isGW, connected, cn, _minGWs, true); // Run fc. obj. computing channels and UF avg.
    if(_channels >= MAX_CHANNELS && _sfmax > 7 && _exitCond != 2){
        _sfmax--;
        printf("Number of channels = %d. Reducing SF Max. to %d.\n", _channels, _sfmax);
        optimizeGA();
    }
}



/////////// GREEDY ///////////

void buildMinSFMatrixGreedy(){            
    for (unsigned int i = 0; i < _enddevices; i++){
        for (unsigned int j = i; j < _enddevices; j++){    
            float distance = computeDistance(i, j);
            if(distance < 62.5)
                _adj_matrix[i][j] = 7;
            else if(distance < 125)
                _adj_matrix[i][j] = 8;
            else if(distance < 250)
                _adj_matrix[i][j] = 9;
            else if(distance < 500)
                _adj_matrix[i][j] = 10;
            else if(distance < 1000)
                _adj_matrix[i][j] = 11;
            else if(distance < 2000)
                _adj_matrix[i][j] = 12;
            else // No SF for this distance
                _adj_matrix[i][j] = 13;
        }
    }
}

void destroyMinSFMatrixGreedy(){
    _adj_matrix.clear();
}

void greedy(){
    vector<int> init,cero;
    vector<vector<unsigned int>> asignados;
    vector<unsigned int> parcial, GWS;
    
    unsigned int i,j;
    vector<int>::iterator it;
    long Best[_enddevices];
    long best, Max;
    srand(time(NULL) ^ (getpid()<<16));
    
    float **Usf = (float**) malloc(sizeof(float*)*6);
    for(int sf = 0; sf < 6; sf++){
        Usf[sf] = (float*) malloc(sizeof(float)*_enddevices);
        for(unsigned int k = 0; k < _enddevices; k++){
            Usf[sf][k] = 0;
        }
    }
    
    cout << "Calcula adyacencia" << endl;
    for (i = 0; i<_enddevices; i++)
        init.push_back(0);
    for (i = 0; i<_enddevices; i++)
        _adj_matrix.push_back(init);
    cero = init;
    buildMinSFMatrixGreedy();
    
    //Para seleccionar el mejor nodo.
    cout << "Inicia Asignación" << endl;
    bool sigo = true;
    unsigned int gw = 0;
    while (sigo) {

        if(getElapsed() > _timeout){
            printf("Timeout.\n");
            _exitCond = 2;
            break;
        }

        gw++;    
        //cout<<"Paso "<<gw<<endl;
        best = 0;
        Max = 0;

        for(i = 0;i < _adj_matrix.size(); i++) {
            Best[i] = 0;
            Best[i] = _adj_matrix[i].size() - count(_adj_matrix[i].begin(), _adj_matrix[i].end(), 0);
            if (Best[i] > Max){
                best = i;
                Max = Best[i];
            }
        }

        GWS.push_back(best);
        for(int k = 0; k < 6; k++)
            Usf[k][best]=0;
        
        for(j = 0; j < _adj_matrix.size(); j++){
            for(int k = 0; k < 6; k++){
                if (_adj_matrix[best][j] == k+7){
                    const float pw = pow(2, k);
                    const float requiredUF = pw / ((float) _network->at(j).period - pw);
                    if (Usf[k][best] + requiredUF < 1){
                        Usf[k][best] += requiredUF;                        
                        parcial.push_back(j);
                        for(unsigned int h = 0; h < _adj_matrix.size(); h++)
                            _adj_matrix[h][j] = 0;
                    }
                }    
            }
        }
        for (unsigned int h = 0; h < parcial.size(); h++)
            _adj_matrix[parcial[h]] = cero;
        
        asignados.push_back(parcial);
        parcial.clear();
        sigo = false;
        for(i = 0; i < _adj_matrix.size(); i++){
            if(_adj_matrix[i].size() - count(_adj_matrix[i].begin(), _adj_matrix[i].end(), 0) > 0){
                sigo = true;
                break;
            }
        }
    }

    cout << "Gateways Necesarios " << gw << endl;

    destroyMinSFMatrixGreedy(); // Release some memory
    buildMinSFMatrix();
    // Compute solution stats (coverage, channels, GW count, etc)
    for(i = 0; i < _enddevices; i++)
        _bestSol[i] = count(GWS.begin(), GWS.end(), i) > 0; // If index is GW
    unsigned int cn;
    bool connected[_enddevices];
    moFunction(_bestSol, connected, cn, _minGWs, true); // Run fc. obj. computing channels and UF avg.
    if(_channels >= MAX_CHANNELS && _sfmax > 7 && _exitCond != 2){
        _sfmax--;
        printf("Number of channels = %d. Reducing SF Max. to %d.\n", _channels, _sfmax);
        destroyMinSFMatrix(); // Destroy matrix before running algorithm again
        greedy();
    }else{
        destroyMinSFMatrix();
    }
}



/////////// SPRINGS ///////////

void improveGWPos(bool* gwPos, bool* connectedEd) {
    /*
        Implements the pseudo-springs method on binary arrays
    */

    // Initialize vector of forces
    struct force {
        unsigned int gwIdx;
        float x;
        float y;
    };
    vector<force> forces;
    //unsigned int ncCount = 0; // Count not connected ED
    for(unsigned int i = 0; i < _enddevices; i++){
        if(gwPos[i]){ // If gw in node i
            forces.push_back({i, 0.0, 0.0});
        }
        //if(!connectedEd[i])
        //    ncCount++;
    }

    // Compute forces over each GW
    for(unsigned int i = 0; i < _enddevices; i++){
        if(!connectedEd[i]){
            for(unsigned int j = 0; j < forces.size(); j++){
                float dx, dy;
                computeDirectionVector(i, forces[j].gwIdx, dx, dy);
                // TODO: divide components with distance
                const float d = sqrt(dx*dx + dy*dy);
                forces[j].x += dx / (d < .1 ? .1 : d);
                forces[j].y += dy / (d < .1 ? .1 : d);
                //cout << d << " " << forces[j].x << " " << forces[j].y << endl;
            }
        }
    }

    // For each GW, determine which node to move next
    for(unsigned int j = 0; j < forces.size(); j++){
        const unsigned int gwId = forces[j].gwIdx;
        const float nextX = _network->at(gwId).x + forces[j].x;
        const float nextY = _network->at(gwId).y + forces[j].y;
        const unsigned int nextId = getClosestId(nextX, nextY);
        if(!gwPos[nextId]){ // Only move to empty slots
            gwPos[gwId] = false;
            gwPos[nextId] = true;
        } // If cant, then stay in place
    }
}

void springs() {
    /*
        Search optima generating solutions where GWs are moved using pseudo-spring
        algorithm
    */
    _exitCond = 0;
    unsigned int minCost = _alpha*_enddevices; // Initially largest cost
    _minGWs = _enddevices;
    bool sol[_enddevices];
    unsigned int gws = _initialGW; // Number of gw
    unsigned int gwsCnt = 0; // Number of gw (computed, but it should be gwsCnt == gws)
    unsigned int cn = 0; // connected EDs (determined when evaluating objective fc).    
    bool connected[_enddevices]; // List of connected ed (after evaluating sol)
    setRandomSol(sol, gws); // Initially random positions
    for(_currentIter = 0; _currentIter < _itermax; _currentIter++){
        moFunction(sol, connected, cn, gwsCnt, false);
        const unsigned int cost = evalCost(cn, gwsCnt);
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
            if(gws < _enddevices){
                gws++;
                addRandomGW(sol);
            }
        }
        if(getElapsed() > _timeout){
            printf("Timeout.\n");
            _exitCond = 2;
            break;
        }

        // Update GW positions (sol will change after this step)
        improveGWPos(sol, connected);
    }
    _currentIter = _itermax;
    
    printf("Min. GW num is %d (cost=%d)\n", _minGWs, minCost);    

    // Check number of channels and restart optimization if it is greater than 16
    moFunction(_bestSol, connected, cn, _minGWs, true); // Run fc. obj. computing channels and UF avg.
    if(_channels >= MAX_CHANNELS && _sfmax > 7 && _exitCond != 2){
        _sfmax--;
        printf("Number of channels = %d. Reducing SF Max. to %d.\n", _channels, _sfmax);
        springs();
    }
};


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
        /// GA PARAMS
        if(strcmp(argv[i], "-pop") == 0){
            if(i+1 < argc)
                _pop_size = atoi(argv[i+1]);
            else
                printHelp();
        }
        if(strcmp(argv[i], "-cros") == 0){
            if(i+1 < argc)
                _cross_frac = atof(argv[i+1]);
            else
                printHelp();
        }
        if(strcmp(argv[i], "-mut") == 0){
            if(i+1 < argc)
                _mut_rate = atof(argv[i+1]);
            else
                printHelp();
        }
    }

    _begin = steady_clock::now();

    if(buildNetwork)
        buildInstance();

    if(_algo != 2){ // For greedy, dist matrix is built later
        buildMinSFMatrix();
        printf("Min. SF matrix built (t = %ld ms).\n", getElapsed());
    }
    _bestSol = (bool*) malloc(sizeof(bool)*_enddevices);

    printf("--------------------------------\n");
    printf("Optimizer started:\n");   
    printf("  Algorithm used: %s\n", getAlgorithmName(_algo).c_str());     
    printf("  Max. SF: %d\n", _sfmax);
    if(_algo == 0){
       printf("  GA Population size: %d\n", _pop_size);        
       printf("  GA crossover fraction: %f\n", _cross_frac);
       printf("  GA mutation rate: %f\n", _mut_rate);
    }
    if(_algo == 1){
        printf("  Initial number of GW: %d\n", _initialGW);        
        printf("  Add gateway after: %d iterations\n", _increaseGWAfter);
    }
    printf("  Max. iterations: %d\n", _itermax);
    printf("  Timeout: %d s\n", _timeout/1000);
    printf("--------------------------------\n\n");

    if(_algo == 0)
        optimizeGA();

    if(_algo == 1)
        optimizeRandomHeuristic();

    if(_algo == 2)
        greedy();

    if(_algo == 3)
        springs();

    // Print elapsed time
    printf("Total elapsed time is %ld ms.\n", getElapsed());
    printSummary();
    
    // Free pointers    
    if(_algo != 2){
        destroyMinSFMatrix(); 
    }
    free(_bestSol);
    _network->clear();
    delete _network;    

    return 0;
}

