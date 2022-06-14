#ifndef NORMAL_H
#define NORMAL_H

#include <math.h>
#include "random.h"

// Normal normal = Normal(0, 1);
// double rnd = normal.random(); // rnd is a random number with normal distribution

class Normal : public Random {
    public:
        Normal(double mean, double stdDev);
        double random();
        int randomInt();
        void setRandom(double &x, double &y);
        void setRandomInt(int &x, int &y);
    private:
        double mean;
        double stdDev;
};

#endif