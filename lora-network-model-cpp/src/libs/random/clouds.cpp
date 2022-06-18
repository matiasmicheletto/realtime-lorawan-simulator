#include "clouds.h"

Clouds::Clouds(Builder builder) : Random() {
    this->clds = builder.clds;
}

Clouds::~Clouds() {
    
}

Clouds::Builder* Clouds::Builder::addCentroid(Random posGenerator, double stdDev){
    double x, y;
    posGenerator.setRandom(x, y);
    clds.push_back({Normal(x, stdDev), Normal(y, stdDev)});
    return this;
}

Clouds Clouds::Builder::build() {
    return Clouds(*this);
}

void Clouds::setRandom(double &x, double &y) {
    int centroidIndex = rand() % (int) clds.size();
    x = clds[centroidIndex][0].random();
    y = clds[centroidIndex][1].random();
}

void Clouds::setRandomInt(int &x, int &y) {
    int centroidIndex = rand() % (int) clds.size();
    x = clds[centroidIndex][0].randomInt();
    y = clds[centroidIndex][1].randomInt();
}