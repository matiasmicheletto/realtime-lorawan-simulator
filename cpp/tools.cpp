#include "tools.h"
#include <stdlib.h>
#include <math.h>

inline int gcd(int a, int b) {    
    return b == 0 ? a : gcd(b, a % b);
}

template <typename T> T randomSelect(const T *array, const int size) {
    return array[rand() % size];
}

template <typename T> T randomSelect(const T *array, const double *prob, const int size) {
    double rnd = (double)rand() / RAND_MAX;
    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += prob[i];
        if (rnd < sum)
            return array[i];
    }
    return array[size - 1];
}

double uniformDist() {
    return (double)rand() / (double)RAND_MAX - 0.5;
}

double normalDist(const double mean = 0, const double stdDev = 0.2) {
    double u1 = uniformDist();
    double u2 = uniformDist();
    double randStdNormal = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
    return mean + stdDev * randStdNormal;
}
