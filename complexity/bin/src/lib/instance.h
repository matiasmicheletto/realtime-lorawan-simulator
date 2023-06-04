
#ifndef INSTANCE_H
#define INSTANCE_H


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
        
        inline int getGWCount(){return this->gwCount;}
        inline int getEDCount(){return this->edCount;}

    private:
        int gwCount, edCount;
        vector<vector<int>> raw;
};

#endif // INSTANCE_H
