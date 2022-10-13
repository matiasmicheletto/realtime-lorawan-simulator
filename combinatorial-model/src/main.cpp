#include <iostream>
#include <vector>
#include <limits>
#include <random>
#include <functional>
#include <chrono>
#include <math.h>
#include <string.h>
#include <stdio.h>

#define LINE_BUFFER_SIZE 256 // Max length for csv file line
#define FILE_COLS 4 // Columns of csv file (id, x, y, period)
#define INPUT_FILE_NAME "input.csv" // Input file name

using namespace std;
using namespace std::chrono;

void printHelp() {    
    printf("Usage: ./runnable [-h | --help] [-i | --input] [-o | --output] [-a | --alpha] [-b | --beta]\n");
    
    printf("Arguments:\n");
    printf("  -h, --help            prints this help and exits.\n");
    printf("  -i, --input           network configuration file.\n");        
    printf("  -o, --output          output file name.\n");
    printf("  -a, --alpha           alpha tunning parameter.\n");    
    printf("  -b, --beta            beta tunning parameter.\n");    
    
    
    printf("Default values:\n");
    printf("  -i, --input           input.csv\n");
    printf("  -o, --output          output.dat\n");
    printf("  -a, --alpha           100.\n");    
    printf("  -b, --beta            1.\n");    

    printf("Examples: \n");
    printf("\t./runnable -i input.csv -o output.dat -a 1000 -b 5\n");
    exit(1);
}

// Utils
inline int gcd(int a, int b) {return b == 0 ? a : gcd(b, a % b);}
inline int lcm(int a, int b){return a * b / gcd(a, b);}

struct node { // End devices
    unsigned int id;
    float x;
    float y;
    unsigned int period;
    unsigned char maxSF; // Computed from period
};

struct gw { // Gateways
    unsigned int id;
    float uf[6];
};

// Binary random generator
auto binRnd = bind(uniform_int_distribution<>(0,1), default_random_engine());

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

// Load network from file (file.csv -> vector<node>*)
vector<node>* loadInstance(const char* filename, unsigned int &H) { 
    /*
        This function parses input csv file, builds a vector of nodes 
        and returns a pointer to it.
        A second argument &H (pointer to unsigned int H) is taken to 
        compute the value of the resulting hyperperiod.
    */
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file csv file\n");        
        exit(1);
    }

    H = 1;
    vector<node> *network = new vector<node>;
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
                    H = (unsigned int) lcm(H, period);
                    break;
                default:
                    break;
            }
        }
        network->push_back({id, x, y, period, maxSF});
    }
    fclose(file);

    return network;
}

// Euclidean distance between nodes i and j (computationally expensive, use memoization)
float computeDistance(const vector<node>* nodes, const unsigned int i, const unsigned int j) { 
    /*
        This function computes the euclidean distance between nodes i and j.
        i and j are the indexes of vector of nodes.
        It is assumed that:         
        0 <= i < N and 
        0 <= j < N.
    */
    float dx = nodes->at(i).x - nodes->at(j).x;
    float dy = nodes->at(i).y - nodes->at(j).y;
    return sqrt(dx*dx + dy*dy);
}

// Compute the min SF matrix for network nodes
unsigned char** buildMinSFMatrix(const vector<node>* nodes) {
    /*
        Builds the minimum SF matrix and returns a pointer to it.
        matrix[i][j] means min SF for connecting nodes (i+1) and j.
        INPORTANT: This matrix is triangular, so i,j: i > j
    */
    const unsigned int N = nodes->size();
    unsigned char** matrix = (unsigned char**) malloc(sizeof(unsigned char*)*N - 1);
    for(unsigned int i = 0; i < N-1; i++){
        matrix[i] = (unsigned char*) malloc(sizeof(unsigned char)*(i+1));
        for(unsigned int j = 0; j < i+1; j++){
            float distance = computeDistance(nodes, i+1, j);
            if(distance < 62.5)
                matrix[i][j] = 7;
            else if(distance < 125)
                matrix[i][j] = 8;
            else if(distance < 250)
                matrix[i][j] = 9;
            else if(distance < 500)
                matrix[i][j] = 10;
            else if(distance < 1000)
                matrix[i][j] = 11;
            else if(distance < 2000)
                matrix[i][j] = 12;
            else // No SF for this distance
                matrix[i][j] = 13;
        }
    }
    return matrix;
}

// Access min SF matrix of network
unsigned char getMinSF(unsigned char** matrix, const unsigned int i, const unsigned int j) {    
    /*
        This function eases the acces to the min SF matrix.
        0 <= i < N and 
        0 <= j < N.
    */
    if(i == j) return 7; // If gw is on ed position, sf=7
    return i < j ? matrix[j-1][i] : matrix[i-1][j];
}

void printMinSFMatrix(unsigned char** matrix, unsigned int size) {
    /*
        This function is intended to test the computing of the min. SF matrix.
    */
    for(unsigned int i = 0; i < size; i++){
        for(unsigned int j = 0; j < size; j++){
            if(i != j)
                printf("%d ", getMinSF(matrix, i, j));
            else 
                printf("%d ", 7);
        }
        printf("\n");
    }
}

// Free pointers of min SF matrix
void destroyMinSFMatrix(unsigned char** matrix, const unsigned int size) {
    for(unsigned int i = 0; i < size-1; i++)
        free(matrix[i]);
    free(matrix);
}

unsigned int objectiveFunction( // Get cost of a candidate solution
    const vector<node>* nodes, // List of nodes of network
    unsigned char** minSFMatrix, // Min. SF matrix
    const bool *sol, // Pointer to candidate solution to evaluate
    unsigned int alpha, // Tunning parameter (not connected ed)
    unsigned int beta, // Tunning parameter (number of gw)
    unsigned int &connectedNodes // Resulting connected end devices
    ) {
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
    const unsigned int size = nodes->size();
    vector<gw> *gateways = new vector<gw>;
    bool* connected = (bool*) malloc(sizeof(bool)*size);
    for(unsigned int i = 0; i < size; i++){
        connected[i] = false;
        if(sol[i]){ // If gw
            gateways->push_back({i, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}}); // Init gw empty UF            
        }
    }
    connectedNodes = 0;

    if(gateways->size() == 0)
        return alpha*size;

    for(unsigned char sf = 7; sf <= 12; sf++){ // For each SF (7..12)
        for(unsigned int i = 0; i < size; i++){ // For each node
            if(!connected[i]){ // If node is not connected
                for(unsigned int j = 0; j < gateways->size(); j++){ // For each gateway
                    // Check if feasible sf (before computing UF)
                    if(sf >= getMinSF(minSFMatrix, i, gateways->at(j).id) && sf <= nodes->at(i).maxSF){ 
                        const float requiredUF = pow(2, sf-7) / ((float) nodes->at(i).period - pow(2,sf-7));
                        if(gateways->at(j).uf[sf-7] < 1.0 - requiredUF){ // If possible to connect
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
    const unsigned int cost = alpha*(size - connectedNodes) + beta*gateways->size();

    // Free used memory
    free(connected);
    gateways->clear();
    delete gateways;
    
    return cost;
}

void printSol(bool *sol, unsigned int size){
    for(unsigned int i = 0; i < size; i++)
        printf("%d ",sol[i]);
    printf("\n");
}

void setRandomSol(bool *sol, unsigned int size){
    /*
        Generates a full random binary array
    */
    for(unsigned int i = 0; i < size; i++)
        sol[i] = (bool) binRnd();
}

void setRandomSolN(bool *sol, unsigned int size, unsigned int N) {
    /*
        Generates a random binary array with only N ones (at random positions)
    */
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    uniform_int_distribution<> distrib(0, size);
    for(unsigned int i = 0; i < size; i++)
        sol[i] = false; // Set all to false
    for(unsigned int i = 0; i < N; i++){
        unsigned int pos = (unsigned int) distrib(gen);
        while(sol[pos]) // TODO: In case of "N" close to "size", this could take a while
            pos = (unsigned int) distrib(gen);
        sol[pos] = true;
    }
}

void optimizeFullRandom(
    const vector<node>* network, // List of nodes of network
    unsigned char** minSFMatrix, // Min. SF matrix
    unsigned int iterMax, // Max iterations
    unsigned int alpha, // Tunning parameter (not connected ed)
    unsigned int beta // Tunning parameter (number of gw)
    ) {
    /*
        Search optima generating full random solutions
    */
    unsigned int minCost = network->size();
    bool* sol = (bool*) malloc(sizeof(bool)*network->size());
    unsigned int cn = 0; // connected nodes when evaluating objective fc.
    for(unsigned int iter = 0; iter < iterMax; iter++){
        setRandomSol(sol, network->size()); // Full random generator
        unsigned int cost = objectiveFunction(network, minSFMatrix, sol, alpha, beta, cn);
        if(cost < minCost){
            minCost = cost;
            printf("New min. cost found %d on iter %d (%d connected)\n", minCost, iter, cn);
            printSol(sol, network->size());
        }
    }
    
    printf("Min cost computed is %d \n", minCost);
    free(sol);
}

void optimizeRandomHeuristic(
    const vector<node>* network, // List of nodes of network
    unsigned char** minSFMatrix, // Min. SF matrix
    unsigned int iterMax, // Max iterations
    unsigned int initialGW, // Initial number of GW
    unsigned int increaseAfter, // Iterations to increase the number of GW
    unsigned int alpha, // Tunning parameter (not connected ed)
    unsigned int beta // Tunning parameter (number of gw)
    ) {
    /*
        Search optima generating random solutions, but having control
        of the number of GW
    */
    unsigned int minCost = network->size();
    bool* sol = (bool*) malloc(sizeof(bool)*network->size());    
    unsigned int cn = 0; // connected nodes when evaluating objective fc.
    unsigned int gw = initialGW;
    unsigned int incrCntr = 0;
    for(unsigned int iter = 0; iter < iterMax; iter++){
        setRandomSolN(sol, network->size(), gw); // Full random generator
        unsigned int cost = objectiveFunction(network, minSFMatrix, sol, alpha, beta, cn);
        if(cost < minCost){
            minCost = cost;
            printf("New min. found %d gw (cost=%d) on iter %d (%d connected)\n", gw, minCost, iter, cn);
            //printSol(sol, network->size());
            if(cn == network->size()) // Once all connected, try to reduce gw number
                gw--;
        }
        
        incrCntr++;
        if(incrCntr > increaseAfter){
            if(gw < network->size())
                gw++;
            incrCntr = 0;
        }
    }
    
    printf("Min cost computed is %d \n", minCost);
    free(sol);
}

int main(int argc, char **argv) {

    default_random_engine generator( random_device{}() ); // Random seed
    char* inputfilename;
    char* outputfilename;
    unsigned int alpha = 1000;
    unsigned int beta = 1;

    for(int i = 0; i < argc; i++) {
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
            printHelp();
        }
        if(strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) {
            if(i + 1 < argc) {
                inputfilename = argv[i + 1];
            }else
                printHelp();
        }
        if(strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if(i + 1 < argc) {
                outputfilename = argv[i + 1];
            }else
                printHelp();
        }
        if(strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--alpha") == 0){
            if(i+1 < argc)
                alpha = atoi(argv[i+1]);
            else
                printHelp();
        }
        if(strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--beta") == 0){
            if(i+1 < argc)
                beta = atoi(argv[i+1]);
            else
                printHelp();
        }        
    }

    unsigned int H;
    vector<node> *network = loadInstance(inputfilename, H);

    printf("-----------------------------------\n");
    printf("Configuration file loaded\n");
    printf("End devices: %ld\n", network->size());
    printf("System hyperperiod: %d\n", H);        
    printf("-----------------------------------\n\n");

    steady_clock::time_point begin = steady_clock::now();

    unsigned char** minSF = buildMinSFMatrix(network);
    printf("Min. SF matrix built.\n");
    //printf("Min. SF matrix:\n");
    //printMinSFMatrix(minSF, network->size()); // Print minSF 
    //printf("-----------------------------------\n\n");
    
    //optimizeFullRandom(network, minSF, 100, alpha, beta);
    optimizeRandomHeuristic(network, minSF, 5000, 1, 50, alpha, beta);
    
    // Print elapsed time
    steady_clock::time_point end = steady_clock::now();
    unsigned long int elapsed = (unsigned long int) duration_cast<milliseconds>(end - begin).count();
    printf("Elapsed time is %ld ms.\n", elapsed);
    
    // Free pointers    
    destroyMinSFMatrix(minSF, network->size()); 
    network->clear();
    delete network;

    return 0;
}

