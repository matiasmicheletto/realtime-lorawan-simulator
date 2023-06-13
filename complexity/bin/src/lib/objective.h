#ifndef OBJECTIVE_H
#define OBJECTIVE_H

/* 
    Class Objective: Models problem's objective function. Takes a candidate solution
    and returns its objective value. Requires problem model (instance). 
    Optimization variable format:
        -- x[i][0]: GW for ED i.
        -- x[i][1]: SF used by ED i.
*/

#include <math.h>
#include "instance.h"

// UF value for a GW to count it as used
#define UFTHRES 1e-6

enum VINDEX {GW, SF};
enum T_PARAMS {ALPHA, BETA, GAMMA};

class Objective {
    public:
        Objective(Instance* instance);
        ~Objective();
        
        double eval(unsigned int** x, unsigned int &gwCount, double &energy, double &totalUF);
        double params[3];
        void printSolution(unsigned int** x);
    private:
        Instance* instance;
};

#endif // OBJECTIVE_H