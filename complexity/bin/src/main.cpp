#include <string>
#include <cstring>
#include <stdio.h>

#include "lib/instance.h"
#include "lib/objective.h"


void printHelp() {    
    std::cout << "Usage: ./binary [-f | --file]\n" << std::endl;
    
    std::cout << "Arguments:\n" << std::endl;
    std::cout << "  -f, --file            input file.\n" << std::endl;

    std::cout << "Example: \n" << std::endl;
    std::cout << "\t./binary -f instance.dat\n" << std::endl;    
    exit(1);
}

int main(int argc, char **argv) {
    
    Instance *l = 0;
    double alpha = 1.0, beta = 1.0, gamma = 1.0;
    
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
    std::cout << "Raw data:" << std::endl;
    l->printRawData();
    std::cout << std::endl;
    std::cout << "GW Count: " << l->getGWCount() << std::endl;
    std::cout << "ED Count: " << l->getEDCount() << std::endl;

    // Generate candidate solution 
    unsigned int **x = (unsigned int**) malloc( sizeof(unsigned int*) * l->getEDCount());
    for(unsigned int i = 0; i < l->getEDCount(); i++)
        x[i] = (unsigned int*) malloc( sizeof(unsigned int) * 2 );
    x[0][VINDEX::GW] = 0;
    x[0][VINDEX::SF] = 7;
    x[1][VINDEX::GW] = 1;
    x[1][VINDEX::SF] = 7;
    x[2][VINDEX::GW] = 2;
    x[2][VINDEX::SF] = 7;
    x[3][VINDEX::GW] = 2;
    x[3][VINDEX::SF] = 7;
    x[4][VINDEX::GW] = 3;
    x[4][VINDEX::SF] = 7;
    x[5][VINDEX::GW] = 3;
    x[5][VINDEX::SF] = 9;

    std::cout << "Tunning parameters:" << std::endl;
    std::cout << "Alpha: " << alpha << std::endl;
    std::cout << "Beta: " << beta << std::endl;
    std::cout << "Gamma: " << gamma << std::endl;

    // Eval generated solution (candidate)
    Objective *o = new Objective(l);
    o->params[T_PARAMS::ALPHA] = alpha;
    o->params[T_PARAMS::BETA] = beta;
    o->params[T_PARAMS::GAMMA] = gamma;

    const int result = o->eval(x);
    std::cout << "Result: " << result << std::endl;
    
    // Release memory
    for(unsigned int i = 0; i < l->getEDCount(); i++)
        free(x[i]);
    free(x);
    delete o;
    delete l;
    l = 0; 
    o = 0;
    
    return 0;
}

