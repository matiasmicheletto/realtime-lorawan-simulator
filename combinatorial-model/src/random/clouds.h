#ifndef CLOUDS_H
#define CLOUDS_H

#include <vector>
#include "random.h"
#include "uniform.h"
#include "normal.h"

// Cloud cloud = Cloud(-500, 500, 5)
// float x, y;
// cloud.setRandom(x, y); // x and y are random coordinates from one of the clouds

class Clouds : public Random {
    public:
        Clouds(float min, float max, int numCentroids);
        ~Clouds();
        void setRandom(float &x, float &y);
        void setRandomInt(int &x, int &y);
    private:
        std::vector<std::vector<Normal>> clds;
};

#endif // CLOUDS_H