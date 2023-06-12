#ifndef INSTANCE_H
#define INSTANCE_H

/* 
    Class Instance: Models problem parameters. Load parameters from input file 
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;


class Instance {
    public:

        Instance(char* filename);
        ~Instance();
        
        void printRawData();
        
        inline unsigned int getGWCount(){return this->gwCount;}
        inline unsigned int getEDCount(){return this->edCount;}
        unsigned int getSF(int ed, int gw);
        unsigned int getMaxSF(unsigned int period);
        unsigned int getPeriod(int ed);

    private:
        vector<vector<int>> raw;
        void parseRawData();
        unsigned int gwCount, edCount;
};

#endif // INSTANCE_H
