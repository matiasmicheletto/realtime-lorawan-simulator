#include "tools.h"

double uniformDist() {
    return (double)rand() / (double)RAND_MAX - 0.5;
}

double normalDist(const double mean, const double stdDev) {
    double u1 = uniformDist();
    double u2 = uniformDist();
    double randStdNormal = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
    return mean + stdDev * randStdNormal;
}