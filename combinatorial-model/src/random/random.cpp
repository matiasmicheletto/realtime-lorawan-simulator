#include "random.h"

Random::Random() {
}

Random::~Random() {
}

float Random::random() {
    return (float)rand() / (float)RAND_MAX;
}

int Random::randomInt() {
    return rand();
}

void Random::setRandom(float &x, float &y) {
    x = random();
    y = random();
}

void Random::setRandomInt(int &x, int &y) {
    x = randomInt();
    y = randomInt();
}
