#include "normal.h"

Normal::Normal(double min, double max, double mean, double stdDev) : Random() {
    this->min = min;
    this->max = max;
    this->mean = mean;
    this->stdDev = stdDev;
}

double Normal::random() {
    double u = (double)rand() / (double)RAND_MAX;
    double v = (double)rand() / (double)RAND_MAX;
    double randStdNormal = sqrt(-2.0 * log(u)) * cos(2.0 * M_PI * v);
    double scaler = (this->max - this->min) / 2.0;
    double rnd = this->mean + (this->stdDev * randStdNormal)*scaler;
    return rnd < this->min ? this->min : (rnd > this->max ? this->max : rnd);
}

int Normal::randomInt() {
    return (int)this->random();
}

void Normal::setRandom(double &x, double &y) {
    x = this->random();
    y = this->random();
}

void Normal::setRandomInt(int &x, int &y) {
    x = this->randomInt();
    y = this->randomInt();
}