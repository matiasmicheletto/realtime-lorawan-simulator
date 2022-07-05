#ifndef CLOUDS_H
#define CLOUDS_H

#include <vector>
#include "random.h"
#include "uniform.h"
#include "normal.h"

// Cloud cloud = Cloud(-500, 500, 5)
// double x, y;
// cloud.setRandom(x, y); // x and y are random coordinates from one of the clouds

class Clouds : public Random {
    public:
        Clouds(double min, double max, int numCentroids);
        ~Clouds();
        void setRandom(double &x, double &y);
        void setRandomInt(int &x, int &y);
    private:
        std::vector<std::vector<Normal>> clds;
};

#endif // CLOUDS_H