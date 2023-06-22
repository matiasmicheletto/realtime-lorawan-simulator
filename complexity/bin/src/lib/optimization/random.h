#ifndef RANDOMSEARCH_H
#define RANDOMSEARCH_H

#include <chrono>
#include <vector>
#include "../util/util.h"
#include "../random/uniform.h"
#include "../instance/instance.h"
#include "../objective/objective.h"

void randomSearch(Instance* l, Objective* o, unsigned long maxIters);
void improvedRandomSearch(Instance* l, Objective* o, unsigned long maxIters);

#endif // RANDOMSEARCH_H