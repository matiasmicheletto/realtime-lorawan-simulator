#include "objective.h"

Objective::Objective(Instance* instance) {
    this->instance = instance;
}

Objective::~Objective() {

}

double Objective::eval(int **x) {
    return 0.0;
}
