#ifndef CUSTOMDIST_H
#define CUSTOMDIST_H

#include <vector>
#include <stdio.h>
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
                Builder* addValue(float value, float prob);
                Builder* addValue(int value, float prob);
                CustomDist build();
                std::vector<float> values;
                std::vector<float> probs;
        };
        CustomDist(Builder builder);
        ~CustomDist();
        float random();
        int randomInt();
        void setRandom(float &x, float &y);
        void setRandomInt(int &x, int &y);
    private:
        std::vector<float> values;
        std::vector<float> probs;
};

#endif // CUSTOMDIST_H