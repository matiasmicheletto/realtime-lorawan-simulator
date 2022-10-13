#include "normal.h"

Normal::Normal(float min, float max, float mean, float stdDev) : Random() {
    this->min = min;
    this->max = max;
    this->mean = mean;
    this->stdDev = stdDev;
}

float Normal::random() {
    float u = (float)rand() / (float)RAND_MAX;
    float v = (float)rand() / (float)RAND_MAX;
    float randStdNormal = sqrt(-2.0 * log(u)) * cos(2.0 * M_PI * v);
    float scaler = (this->max - this->min) / 2.0;
    float rnd = this->mean + (this->stdDev * randStdNormal)*scaler;
    return rnd < this->min ? this->min : (rnd > this->max ? this->max : rnd);
}

int Normal::randomInt() {
    return (int)this->random();
}

void Normal::setRandom(float &x, float &y) {
    x = this->random();
    y = this->random();
}

void Normal::setRandomInt(int &x, int &y) {
    x = this->randomInt();
    y = this->randomInt();
}