#include <string>
#include <cstring>
#include <fstream>
#include <chrono>
#include <stdio.h>

#include "lib/util.h"
#include "lib/instance.h"
#include "lib/objective.h"
#include "lib/random/uniform.h"


void printHelp() {    
    std::ifstream manualFile("readme.txt");
    if (manualFile.is_open()) {
        std::string line;
        while (getline(manualFile, line)) {
            std::cout << line << std::endl;
        }
        manualFile.close();
        exit(1);
    } else {
        std::cerr << "Error: Unable to open manual file." << std::endl;
    }
}

int main(int argc, char **argv) {
    
    Instance *l = 0;
    double alpha = 1.0, beta = 1.0, gamma = 1.0;
    unsigned long maxIters = 1e6;
    
    // Program arguments
    for(int i = 0; i < argc; i++) {    
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            printHelp();

        if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
            if(i+1 < argc) 
                l = new Instance(argv[i + 1]);
            else
                printHelp();
        }
        if(strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--iter") == 0) {
            if(i+1 < argc) 
                maxIters = atoi(argv[i+1]);
            else
                printHelp();
        }
        if(strcmp(argv[i], "-alpha") == 0){
            if(i+1 < argc)
                alpha = atof(argv[i+1]);
            else
                printHelp();
        }
        if(strcmp(argv[i], "-beta") == 0){
            if(i+1 < argc)
                beta = atof(argv[i+1]);
            else
                printHelp();
        }
        if(strcmp(argv[i], "-gamma") == 0){
            if(i+1 < argc)
                gamma = atof(argv[i+1]);
            else
                printHelp();
        }
    }

    if(l == 0) printHelp();

    // If valid instance passed as input -> proceed
    std::cout << "Raw data loaded from input file:" << std::endl;
    l->printRawData();
    std::cout << std::endl;
    std::cout << "GW Count: " << l->getGWCount() << std::endl;
    std::cout << "ED Count: " << l->getEDCount() << std::endl;

    
    Objective *o = new Objective(l);
    o->params[T_PARAMS::ALPHA] = alpha;
    o->params[T_PARAMS::BETA] = beta;
    o->params[T_PARAMS::GAMMA] = gamma;

    std::cout << "-------------------" << std::endl;
    std::cout << "Tunning parameters:" << std::endl;
    std::cout << "Alpha: " << alpha << std::endl;
    std::cout << "Beta: " << beta << std::endl;
    std::cout << "Gamma: " << gamma << std::endl << std::endl;

    /// RANDOM SEARCH
    unsigned int **x = (unsigned int**) malloc( sizeof(unsigned int*) * l->getEDCount());
    unsigned int **best = (unsigned int**) malloc( sizeof(unsigned int*) * l->getEDCount());
    for(unsigned int i = 0; i < l->getEDCount(); i++){
        x[i] = (unsigned int*) malloc( sizeof(unsigned int) * 2 );
        best[i] = (unsigned int*) malloc( sizeof(unsigned int) * 2 );
    }

    srand(time(NULL));
    Uniform gwGenerator = Uniform(0, l->getGWCount());
    Uniform sfGenerator = Uniform(7, 12);

    double bestQ = 0.0;

    std::cout << "Running " << maxIters << " iterations..." << std::endl << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    for(unsigned int k = 0; k < maxIters; k++){ // For each iteration
        // Generate a random solution (candidate)
        for(unsigned int i = 0; i < l->getEDCount(); i++){
            x[i][VINDEX::GW] = gwGenerator.random();
            x[i][VINDEX::SF] = sfGenerator.random();
        }
        // Test generated solution
        unsigned int gwCount;
        double energy, totalUF;
        const double q = o->eval(x, gwCount, energy, totalUF);

        if(q > bestQ){
            bestQ = q;
            arrayCopy(x, best, l->getEDCount(), 2);
            std::cout << "Iteration:" << k << " -- New best found:" << std::endl;
            o->printSolution(best);
            std::cout << "-------------------" << std::endl << std::endl;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Random search finished, " << maxIters << " iterations performed." << std::endl;
    std::cout << "Execution time: " << duration << " milliseconds" << std::endl;

    // Release memory
    for(unsigned int i = 0; i < l->getEDCount(); i++){
        free(x[i]);
        free(best[i]);
    }
    free(x);
    free(best);
    delete o;
    delete l;
    l = 0; 
    o = 0;
    
    return 0;
}
