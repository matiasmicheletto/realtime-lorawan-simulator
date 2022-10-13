#include "clouds.h"

Clouds::Clouds(float min, float max, int numCentroids) : Random() {
    Uniform uniform(min*(float).6, max*(float).6);
    for (int i = 0; i < numCentroids; i++) {
        float x, y;
        uniform.setRandom(x, y);
        clds.push_back({Normal(min, max, x, 0.15), Normal(min, max, y, 0.15)});
    }
}

Clouds::~Clouds() {
    clds.clear();
}

void Clouds::setRandom(float &x, float &y) {
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