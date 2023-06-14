
#include "uniform.h"

Uniform::Uniform(double min, double max) : Random() {
    this->min = min;
    this->max = max;
}

Uniform::Uniform(int min, int max) : Random() {
    this->min = (double)min;
    this->max = (double)max;
}

double Uniform::random() {
    return (double)rand() / (double)RAND_MAX * (this->max - this->min) + this->min;
}

int Uniform::randomInt() {
    return rand() % (int)(this->max - this->min + 1) + this->min;
}

void Uniform::setRandom(double &x, double &y) {
    x = random();
    y = random();
}

void Uniform::setRandomInt(int &x, int &y) {
    x = randomInt();
    y = randomInt();
}