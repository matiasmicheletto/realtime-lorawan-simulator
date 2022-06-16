#ifndef CLOUDS_H
#define CLOUDS_H

#include <vector>
#include "random.h"
#include "normal.h"

// Uniform posGenerator = Uniform(-500, 500);
// Cloud cloud = Cloud::Builder()
//   .addCentroid(posGenerator, 0.1)
//   ->addCentroid(posGenerator, 0.15)
//   ->addCentroid(posGenerator, 0.2)
//   ->build();
// double x, y;
// cloud.setRandom(x, y); // x and y are random coordinates from one of the clouds

class Clouds : public Random {
    public:
        class Builder {
            public:
                Builder* addCentroid(Random posGenerator, double stdDev);
                Clouds build();
                std::vector<std::vector<Normal>> clds;
        };
        Clouds(Builder builder);
        void setRandom(double &x, double &y);
        void setRandomInt(int &x, int &y);
    private:
        std::vector<std::vector<Normal>> clds;
};

#endif // CLOUDS_H