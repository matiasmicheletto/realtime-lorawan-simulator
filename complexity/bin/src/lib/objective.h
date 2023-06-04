#ifndef OBJECTIVE_H
#define OBJECTIVE_H

#include "instance.h"

class Objective {
    public:
        Objective(Instance* instance);
        ~Objective();
        
        double eval(int** x);
    private:
        Instance* instance;
};

#endif // OBJECTIVE_H