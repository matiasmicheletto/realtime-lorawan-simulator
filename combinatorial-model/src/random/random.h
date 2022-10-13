#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>

class Random {
    public:
        Random();
        virtual ~Random();
        virtual float random();
        virtual int randomInt();
        virtual void setRandom(float &x, float &y);
        virtual void setRandomInt(int &x, int &y);
};

#endif // RANDOM_H