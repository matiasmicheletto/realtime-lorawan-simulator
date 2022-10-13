#include "custom.h"

CustomDist::CustomDist(Builder builder) : Random() {
    this->values = builder.values;
    this->probs = builder.probs;
}

CustomDist::~CustomDist() {
    
}

CustomDist::Builder* CustomDist::Builder::addValue(float value, float prob) {
    this->values.push_back(value);
    this->probs.push_back(prob);
    return this;
}

CustomDist::Builder* CustomDist::Builder::addValue(int value, float prob) {
    return this->addValue((float)value, prob);
}

CustomDist CustomDist::Builder::build() {
    // Check if sum of probs equals 1
    float sum = 0;
    for (long unsigned int i = 0; i < this->probs.size(); i++)
        sum += this->probs[i];
    if (sum < 0.999){ 
        printf("Warning [CustomDist]: Sum of probabilities does not equal 1!\n");
        printf("Sum of probabilities: %.2f\n", sum);
    }
    return CustomDist(*this);
}

float CustomDist::random() {
    float r = (float)rand() / (float)RAND_MAX;
    float sum = 0.0;
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

void CustomDist::setRandom(float &x, float &y) {
    x = random();
    y = random();
}

void CustomDist::setRandomInt(int &x, int &y) {
    x = randomInt();
    y = randomInt();
}