#include "normal.h"

Normal::Normal(double mean, double stdDev) : Random() {
    this->mean = mean;
    this->stdDev = stdDev;
}

double Normal::random() {
    double u1 = (double)rand() / (double)RAND_MAX - 0.5;
    double u2 = (double)rand() / (double)RAND_MAX - 0.5;
    double randStdNormal = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
    return mean + stdDev * randStdNormal;
}

int Normal::randomInt() {
    return (int)random();
}

void Normal::setRandom(double &x, double &y) {
    x = random();
    y = random();
}

void Normal::setRandomInt(int &x, int &y) {
    x = randomInt();
    y = randomInt();
}