#ifndef INSTANCE_H
#define INSTANCE_H

/* 
    Class Instance: Models problem parameters. Load parameters from input file 
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>


class Instance {
    public:
        Instance(char* filename);
        ~Instance();
        
        void printRawData();

    private:
        std::vector<std::vector<int>> raw;
        void parseRawData();
};

#endif // INSTANCE_H
