#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>

class Random {
    public:
        Random();
        ~Random();
        virtual double random();
        virtual int randomInt();
        virtual void setRandom(double &x, double &y);
        virtual void setRandomInt(int &x, int &y);
};

#endif // RANDOM_H