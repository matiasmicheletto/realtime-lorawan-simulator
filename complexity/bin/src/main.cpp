#include <string>
#include <cstring>
#include <stdio.h>

#include "lib/instance.h"
#include "lib/objective.h"

using namespace std;

void printHelp() {    
    cout << "Usage: ./binary [-f | --file]\n" << endl;
    
    cout << "Arguments:\n" << endl;
    cout << "  -f, --file            input file.\n" << endl;

    cout << "Example: \n" << endl;
    cout << "\t./binary -f instance.dat\n" << endl;    
    exit(1);
}

int main(int argc, char **argv) {
    
    Instance *l = 0;

    // Program arguments
    for(int i = 0; i < argc; i++) {    
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            printHelp();

        if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
            if(i + 1 < argc) 
                l = new Instance(argv[i + 1]);
            else
                printHelp();
        }
    }

    if(l == 0) printHelp();

    // Valid instance passed as input -> proceed
    cout << "Raw data:" << endl;
    l->printRawData();
    cout << endl;
    cout << "GW Count: " << l->getGWCount() << endl;
    cout << "ED Count: " << l->getEDCount() << endl;

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

    // Eval generated solution (candidate)
    Objective *o = new Objective(l);
    const int result = o->eval(x);
    cout << "Result: " << result << endl;
    
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

