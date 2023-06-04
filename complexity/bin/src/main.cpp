#include "lib/instance.h"
#include <string>
#include <cstring>
#include <stdio.h>

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

    if(l != 0){
        cout << "Raw data:" << endl;
        l->printRawData();
        cout << endl;
        cout << "GW Count: " << l->getGWCount() << endl;
        cout << "ED Count: " << l->getEDCount() << endl;
        delete l;
        l = 0;
    }else{
        printHelp();
    }
    
    return 0;
}

