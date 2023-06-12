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

enum VINDEX {GW, SF};
enum T_PARAMS {ALPHA, BETA, GAMMA};

class Objective {
    public:
        Objective(Instance* instance);
        ~Objective();
        
        double eval(unsigned int** x);
        double params[3];
    private:
        Instance* instance;
};

#endif // OBJECTIVE_H