#include <cstring>

#include "lib/util/util.h"
#include "lib/instance/instance.h"


int main(int argc, char **argv) {
    
    Instance *l = 0;

    // Program arguments
    for(int i = 0; i < argc; i++) {    
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            printHelp();

        if(strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) {
            if(i+1 < argc) 
                l = new Instance(argv[i + 1]);
            else
                printHelp();
        }        
    }

    if(l == 0) printHelp();

    
    std::cout << "Parameters file loaded." << std::endl;
    // l->printRawData();
    // std::cout << std::endl;
    
    delete l;
    l = 0; 
    
    return 0;
}
