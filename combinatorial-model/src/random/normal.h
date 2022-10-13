#ifndef NORMAL_H
#define NORMAL_H

#include <math.h>
#include "random.h"

// Normal normal = Normal(0, 1);
// float rnd = normal.random(); // rnd is a random number with normal distribution

class Normal : public Random {
    public:
        Normal(float min, float max, float mean = 0, float stdDev = 0.3);
        float random();
        int randomInt();
        void setRandom(float &x, float &y);
        void setRandomInt(int &x, int &y);
    private:
        float min;
        float max;
        float mean;
        float stdDev;
};

#endif // NORMAL_H