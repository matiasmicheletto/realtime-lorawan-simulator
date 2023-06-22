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
        
        inline unsigned int getGWCount(){return this->gwCount;}
        inline unsigned int getEDCount(){return this->edCount;}
        unsigned int getMinSF(unsigned int ed, unsigned int gw);
        unsigned int getMaxSF(unsigned int period);
        unsigned int getPeriod(int ed);
        std::vector<unsigned int> getGWList(unsigned int ed);

    private:
        std::vector<std::vector<int>> raw;
        void parseRawData();
        unsigned int gwCount, edCount;
};

#endif // INSTANCE_H
