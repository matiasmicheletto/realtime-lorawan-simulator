#ifndef TOOLS_H
#define TOOLS_H

// Greatest common divisor
inline int gcd(int a, int b) {    
    return b == 0 ? a : gcd(b, a % b);
}

// Least common multiple
inline int lcm(int a, int b){
    return a * b / gcd(a, b);
}

// Select a random element from array
template <typename T> T randomSelect(const T *array, const int size);

// Select a random element from array with defined probability
// For example
// int array[] = {1, 2, 3, 4, 5};
// double prob[] = {0.9, 0.05, 0.05, 0.0, 0.0};
// int result = randomSelect(array, prob, 5);
template <typename T> T randomSelect(const T *array, const double *prob, const int size);

// Uniform distribution generator
double uniformDist();

// Normal distribution generator
double normalDist(const double mean = 0, const double stdDev = 0.2);

#endif