#ifndef CUSTOMDIST_H
#define CUSTOMDIST_H

#include <vector>
#include "random.h"

// CustomDist custom = CustomDist::Builder()
//   .addValue(10, 0.9)
//   ->addValue(20, 0.05)
//   ->addValue(30, 0.05)
//   ->build();
// int rnd = custom.random(); // rnd is a random number with custom distribution


class CustomDist : public Random {
    public:
        class Builder {
            public:
                Builder* addValue(double value, double prob);
                Builder* addValue(int value, double prob);
                CustomDist build();
                std::vector<double> values;
                std::vector<double> probs;
        };
        CustomDist(Builder builder);
        double random();
        int randomInt();
        void setRandom(double &x, double &y);
        void setRandomInt(int &x, int &y);
    private:
        std::vector<double> values;
        std::vector<double> probs;
};

#endif