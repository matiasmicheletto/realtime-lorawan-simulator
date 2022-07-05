#include "random.h"

Random::Random() {
}

Random::~Random() {
}

double Random::random() {
    return (double)rand() / (double)RAND_MAX;
}

int Random::randomInt() {
    return rand();
}

void Random::setRandom(double &x, double &y) {
    x = random();
    y = random();
}

void Random::setRandomInt(int &x, int &y) {
    x = randomInt();
    y = randomInt();
}
