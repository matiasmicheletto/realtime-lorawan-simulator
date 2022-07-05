#ifndef UNIFORM_H
#define UNIFORM_H

#include "random.h"

// Uniform uniform = Uniform(0, 1);
// double rnd = uniform.random(); // rnd is a random number between 0 and 1

class Uniform : public Random {
    public:
        Uniform(double min, double max);
        Uniform(int min, int max);
        double random();
        int randomInt();
        void setRandom(double &x, double &y);
        void setRandomInt(int &x, int &y);
    private:
        double min;
        double max;
};

#endif // UNIFORM_H