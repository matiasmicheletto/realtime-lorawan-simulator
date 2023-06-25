#include "random.h"


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
    bool found = false;

    std::cout << "Running " << maxIters << " iterations..." << std::endl << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    for(unsigned int k = 0; k < maxIters; k++){ // For each iteration
        // Generate a random solution (candidate)
        for(unsigned int i = 0; i < l->getEDCount(); i++){
            x[i][VINDEX::GW] = gwGenerator.random();
            x[i][VINDEX::SF] = sfGenerator.random();
        }
        // Test generated solution
        unsigned int gwCount, energy;
        double totalUF;
        const double q = o->eval(x, gwCount, energy, totalUF);

        if(q < bestQ){ // New optimum
            bestQ = q;
            found = true;
            arrayCopy(x, best, l->getEDCount(), 2);
            std::cout << "Iteration:" << k << " -- New best found:" << std::endl;
            o->printSolution(best);
            std::cout << std::endl << std::endl;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Random search finished, " << maxIters << " iterations performed." << std::endl;
    std::cout << "Execution time: " << duration << " milliseconds" << std::endl;
    if(found){
        std::cout << "Best:" << std::endl;
        o->printSolution(best, true);
    }else{
        std::cout << "Couldn't find a feasible solution for this problem." << std::endl;
    }

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
    bool found = false;

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
        unsigned int gwCount, energy;
        double totalUF;
        const double q = o->eval(x, gwCount, energy, totalUF);

        if(q < bestQ){ // New optimum
            bestQ = q;
            found = true;
            arrayCopy(x, best, l->getEDCount(), 2);
            std::cout << "Iteration:" << k << " -- New best found:" << std::endl;
            o->printSolution(best);
            std::cout << std::endl << std::endl;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Improved random search finished, " << maxIters << " iterations performed." << std::endl;
    std::cout << "Execution time: " << duration << " milliseconds" << std::endl;
    if(found){
        std::cout << "Best:" << std::endl;
        o->printSolution(best, true);
    }else{
        std::cout << "Couldn't find a feasible solution for this problem." << std::endl;
    }

    // Release memory
    for(unsigned int i = 0; i < l->getEDCount(); i++){
        free(x[i]);
        free(best[i]);
    }
    free(x);
    free(best);
}