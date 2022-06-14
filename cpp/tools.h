#ifndef TOOLS_H
#define TOOLS_H

#include <stdlib.h>
#include <math.h>

// Greatest common divisor
inline int gcd(int a, int b) {    
    return b == 0 ? a : gcd(b, a % b);
}

// Least common multiple
inline int lcm(int a, int b){
    return a * b / gcd(a, b);
}

// Select a random element from array
template <typename T> T randomSelect(const T *array, const int size) {
    return array[rand() % size];
}

// Select a random element from array with defined probability
// For example
// int array[] = {1, 2, 3, 4, 5};
// double prob[] = {0.9, 0.05, 0.04, 0.0, 0.0, 1};
// int result = randomSelectProb(array, prob, 5);
template <typename T> T randomSelectProb(const T *array, const double *prob, const int size) {
    double rnd = (double)rand() / RAND_MAX;
    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += prob[i];
        if (rnd < sum)
            return array[i];
    }
    return array[size - 1];
}

// Uniform distribution generator
double uniformDist();

// Normal distribution generator
double normalDist(const double mean = 0.0, const double stdDev = 0.15);

#endif