#ifndef UNIFORM_H
#define UNIFORM_H

#include "random.h"

// Uniform uniform = Uniform(0, 1);
// float rnd = uniform.random(); // rnd is a random number between 0 and 1

class Uniform : public Random {
    public:
        Uniform(float min, float max);
        Uniform(int min, int max);
        float random();
        int randomInt();
        void setRandom(float &x, float &y);
        void setRandomInt(int &x, int &y);
    private:
        float min;
        float max;
};

#endif // UNIFORM_H