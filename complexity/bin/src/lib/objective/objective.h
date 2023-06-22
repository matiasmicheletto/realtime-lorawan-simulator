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
#include "../instance/instance.h"

// UF value for a GW to be considered as "used"
#define UFTHRES .0000001

// Indexes of optimization variable
enum VINDEX {GW, SF}; 

// Tunning parameter names (indexes of params)
enum T_PARAMS {ALPHA, BETA, GAMMA, T_PARAMS_LEN}; 


class Objective {
    public:
        Objective(Instance* instance);
        ~Objective();
        
        double eval(unsigned int** x, unsigned int &gwCount, double &energy, double &totalUF);
        double params[T_PARAMS_LEN];
        void printSolution(unsigned int** x, bool highlight = false);
    private:
        Instance* instance;
};

#endif // OBJECTIVE_H