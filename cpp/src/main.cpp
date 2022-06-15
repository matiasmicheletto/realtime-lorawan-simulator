#include "libs/optimizer/networkoptimizer.h"
#include "libs/random/uniform.h"
#include "libs/random/custom.h"

int main(int argc, char **argv) {

    Uniform posDist = Uniform(-500.0, 500.0);
    CustomDist periodDist = CustomDist::Builder()
        .addValue(3600, 0.97)
        ->addValue(1800, 0.01)
        ->addValue(900, 0.02)
        ->build();

    NetworkOptimizer optimizer = NetworkOptimizer::Builder()
        .setPositionGenerator(&posDist)
        ->setPeriodGenerator(&periodDist)
        ->setMapSize(1000)
        ->setNetworkSize(6500)
        ->setMaxIter(100)
        ->setTimeout(60)
        ->setStepMethod(SPRINGS)
        ->build();

    optimizer.run();
    optimizer.printStatus();

    return 0;
}
