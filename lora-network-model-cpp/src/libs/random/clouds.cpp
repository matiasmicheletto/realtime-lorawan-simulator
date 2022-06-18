#include "clouds.h"

Clouds::Clouds(double min, double max, int numCentroids) : Random() {
    Uniform uniform(min*.6, max*.6);
    for (int i = 0; i < numCentroids; i++) {
        double x, y;
        uniform.setRandom(x, y);
        clds.push_back({Normal(min, max, x, 0.15), Normal(min, max, y, 0.15)});
    }
}

Clouds::~Clouds() {
    clds.clear();
}

void Clouds::setRandom(double &x, double &y) {
    // pick a random index for clds
    int centroidIndex = rand() % (int) clds.size();
    x = clds[centroidIndex][0].random();
    y = clds[centroidIndex][1].random();
}

void Clouds::setRandomInt(int &x, int &y) {
    int centroidIndex = rand() % (int) clds.size();
    x = clds[centroidIndex][0].randomInt();
    y = clds[centroidIndex][1].randomInt();
}