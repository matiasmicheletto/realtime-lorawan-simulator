#ifndef TOOLS_H
#define TOOLS_H

#include <math.h>

// Greatest common divisor
inline int gcd(int a, int b) {    
    return b == 0 ? a : gcd(b, a % b);
}

// Least common multiple
inline int lcm(int a, int b){
    return a * b / gcd(a, b);
}

template <typename T> T mclamp(T value, T min, T max) {
    return value < min ? min : (value > max ? max : value);
}

#endif