#include <cstring>

#include "lib/util/util.h"

#include "lib/instance/instance.h"
#include "lib/objective/objective.h"

#include "lib/optimization/random.h"
#include "lib/optimization/greedy.h"
#include "lib/optimization/ga.h"


int main(int argc, char **argv) {
    
    Instance *l = 0;
    double alpha = 1.0, beta = 1.0, gamma = 1.0;
    unsigned long maxIters = 1e5;
    
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

    
    std::cout << "Input file loaded." << std::endl;
    // l->printRawData();
    // std::cout << std::endl;
    std::cout << "GW Count: " << l->getGWCount() << std::endl;
    std::cout << "ED Count: " << l->getEDCount() << std::endl; 
    std::cout << "Tunning parameters:" << std::endl;
    std::cout << "  Alpha: " << alpha << std::endl;
    std::cout << "  Beta: " << beta << std::endl;
    std::cout << "  Gamma: " << gamma << std::endl << std::endl;


    Objective *o = new Objective(l);
    o->params[T_PARAMS::ALPHA] = alpha;
    o->params[T_PARAMS::BETA] = beta;
    o->params[T_PARAMS::GAMMA] = gamma;

    std::cout << std::endl << "-------------- RS ----------------" << std::endl << std::endl;
    randomSearch(l, o, maxIters);
    
    std::cout << std::endl << "-------------- IRS ---------------" << std::endl << std::endl;
    improvedRandomSearch(l, o, maxIters);
    
    //std::cout << std::endl << "------------- Greedy -------------" << std::endl << std::endl;
    //greedy(l, o);

    std::cout << std::endl << "--------------- GA ---------------" << std::endl << std::endl;
    ga(l, o);
    
    delete o;
    delete l;
    l = 0; 
    o = 0;
    
    return 0;
}
