#include <cstring>
#include <chrono>
#include <vector>

#include "lib/util.h"
#include "lib/instance.h"
#include "lib/objective.h"
#include "lib/random/uniform.h"


void randomSearch(Instance* l, Objective* o, unsigned long maxIters){
    /* Fully random uniformly distributed solutions are generated */

    unsigned int **x = (unsigned int**) malloc( sizeof(unsigned int*) * l->getEDCount());
    unsigned int **best = (unsigned int**) malloc( sizeof(unsigned int*) * l->getEDCount());
    for(unsigned int i = 0; i < l->getEDCount(); i++){
        x[i] = (unsigned int*) malloc( sizeof(unsigned int) * 2 );
        best[i] = (unsigned int*) malloc( sizeof(unsigned int) * 2 );
    }

    Uniform gwGenerator = Uniform(0, l->getGWCount());
    Uniform sfGenerator = Uniform(7, 12);

    double bestQ = __DBL_MAX__; // Cost minimization

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

        if(q < bestQ){ // New optimum
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
    std::cout << "Minimum objective: " << bestQ << std::endl;

    // Release memory
    for(unsigned int i = 0; i < l->getEDCount(); i++){
        free(x[i]);
        free(best[i]);
    }
    free(x);
    free(best);
}

void improvedRandomSearch(Instance* l, Objective* o, unsigned long maxIters) {
    /* Random solutions are generated within feasible SF and GW values */

    unsigned int **x = (unsigned int**) malloc( sizeof(unsigned int*) * l->getEDCount());
    unsigned int **best = (unsigned int**) malloc( sizeof(unsigned int*) * l->getEDCount());
    for(unsigned int i = 0; i < l->getEDCount(); i++){
        x[i] = (unsigned int*) malloc( sizeof(unsigned int) * 2 );
        best[i] = (unsigned int*) malloc( sizeof(unsigned int) * 2 );
    }

    Uniform uniform = Uniform(0.0, 1.0);

    double bestQ = __DBL_MAX__; // Cost minimization

    std::cout << "Running " << maxIters << " iterations..." << std::endl << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    for(unsigned int k = 0; k < maxIters; k++){ // For each iteration
        // Generate a random solution (candidate)
        for(unsigned int i = 0; i < l->getEDCount(); i++){
            std::vector<unsigned int> gwList = l->getGWList(i); // Valid gw for this ed
            x[i][VINDEX::GW] = gwList.at((unsigned int)floor(uniform.random()*gwList.size())); // Pick random gw
            // Pick random SF from valid range
            const unsigned int minSF = l->getMinSF(i, x[i][VINDEX::GW]);
            const unsigned int maxSF = l->getMaxSF(l->getPeriod(i));
            if(minSF > maxSF) // Do not csontinue generating this solution (gw is not valid)
                break;
            x[i][VINDEX::SF] = uniform.random()*(maxSF - minSF) + minSF;
        }
        // Test generated solution
        unsigned int gwCount;
        double energy, totalUF;
        const double q = o->eval(x, gwCount, energy, totalUF);

        if(q < bestQ){ // New optimum
            bestQ = q;
            arrayCopy(x, best, l->getEDCount(), 2);
            std::cout << "Iteration:" << k << " -- New best found:" << std::endl;
            o->printSolution(best);
            std::cout << "-------------------" << std::endl << std::endl;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Improved random search finished, " << maxIters << " iterations performed." << std::endl;
    std::cout << "Execution time: " << duration << " milliseconds" << std::endl;
    std::cout << "Minimum objective: " << bestQ << std::endl;

    // Release memory
    for(unsigned int i = 0; i < l->getEDCount(); i++){
        free(x[i]);
        free(best[i]);
    }
    free(x);
    free(best);
}

int main(int argc, char **argv) {
    
    Instance *l = 0;
    double alpha = 1.0, beta = 1.0, gamma = 1.0;
    unsigned long maxIters = 1e6;
    
    srand(time(NULL));

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


    randomSearch(l, o, maxIters);
    std::cout << "-------------------" << std::endl;
    std::cout << "-------------------" << std::endl;
    improvedRandomSearch(l, o, maxIters);
    
    delete o;
    delete l;
    l = 0; 
    o = 0;
    
    return 0;
}
