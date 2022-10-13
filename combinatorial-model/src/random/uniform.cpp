
#include "uniform.h"

Uniform::Uniform(float min, float max) : Random() {
    this->min = min;
    this->max = max;
}

Uniform::Uniform(int min, int max) : Random() {
    this->min = (float)min;
    this->max = (float)max;
}

float Uniform::random() {
    return (float)rand() / (float)RAND_MAX * (max - min) + min;
}

int Uniform::randomInt() {
    return rand() % (int)(max - min + 1) + min;
}

void Uniform::setRandom(float &x, float &y) {
    x = random();
    y = random();
}

void Uniform::setRandomInt(int &x, int &y) {
    x = randomInt();
    y = randomInt();
}