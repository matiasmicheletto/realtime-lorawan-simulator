#include "custom.h"

CustomDist::CustomDist(Builder builder) : Random() {
    this->values = builder.values;
    this->probs = builder.probs;
}

CustomDist::Builder* CustomDist::Builder::addValue(double value, double prob) {
    values.push_back(value);
    probs.push_back(prob);
    return this;
}

CustomDist::Builder* CustomDist::Builder::addValue(int value, double prob) {
    return addValue((double)value, prob);
}

CustomDist CustomDist::Builder::build() {
    // Check if sum of probs equals 1
    double sum = 0;
    for (long unsigned int i = 0; i < probs.size(); i++)
        sum += probs[i];
    if (sum < 0.999) 
        throw "CustomDist probability fc: Sum of probabilities does not equal 1!";
    return CustomDist(*this);
}

double CustomDist::random() {
    double r = (double)rand() / (double)RAND_MAX;
    double sum = 0.0;
    for (long unsigned int i = 0; i < values.size(); i++) {
        sum += probs[i];
        if (r < sum) {
            return values[i];
        }
    }
    return values[values.size() - 1];
}

int CustomDist::randomInt() {
    return (int)random();
}

void CustomDist::setRandom(double &x, double &y) {
    x = random();
    y = random();
}

void CustomDist::setRandomInt(int &x, int &y) {
    x = randomInt();
    y = randomInt();
}