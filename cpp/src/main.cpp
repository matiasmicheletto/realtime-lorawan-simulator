#include "libs/optimizer/networkoptimizer.h"
#include "libs/random/uniform.h"
#include "libs/random/custom.h"

/*
//Ejemplo minimo:

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
*/

void checkInputs(int argc, char *argv[]) {
    if (argc < 6) {
        printf("Usage: ./runnable map N iter timeout alg\n");
        printf("Examples: \n");
        printf("\t./runnable 1000 6500 100 60 1\n");
        printf("\t./runnable 1000 800 250 120 2\n");
        exit(0);
    }
    if(atoi(argv[1]) < 0 || atoi(argv[1]) > 100000) {
        printf("Map size must be between 0 and 100000\n");
        exit(0);
    }
    if(atoi(argv[2]) < 10 || atoi(argv[2]) > 100000) {
        printf("Network size must be between 10 and 100000\n");
        exit(0);
    }
    if(atoi(argv[3]) < 1 || atoi(argv[3]) > 10000) {
        printf("Maximum number of iterations must be between 1 and 10000\n");
        exit(0);
    }
    if(atoi(argv[4]) < 1 || atoi(argv[4]) > 10000) {
        printf("Timeout must be between 1 and 10000 seconds\n");
        exit(0);
    }
    if(atoi(argv[5]) != 1 && atoi(argv[5]) != 2) {
        printf("Algorithm valid codes are 1 or 2\n");
        exit(0);
    }
}

int main(int argc, char **argv) {

    checkInputs(argc, argv);

    unsigned int mapSize = atoi(argv[1]);
    unsigned int networkSize = atoi(argv[2]);
    unsigned int maxIter = atoi(argv[3]);
    int timeout = atoi(argv[4]);
    STEP_METHOD alg = (STEP_METHOD) atoi(argv[5]);

    Uniform posDist = Uniform(-(double) mapSize/2, (double) mapSize/2);
    CustomDist periodDist = CustomDist::Builder()
        .addValue(3600, 0.97)
        ->addValue(1800, 0.01)
        ->addValue(900, 0.02)
        ->build();

    NetworkOptimizer optimizer = NetworkOptimizer::Builder()
        .setPositionGenerator(&posDist)
        ->setPeriodGenerator(&periodDist)
        ->setMapSize(mapSize)
        ->setNetworkSize(networkSize)
        ->setMaxIter(maxIter)
        ->setTimeout(timeout)
        ->setStepMethod(alg)
        ->build();

    optimizer.run();
    optimizer.printStatus();

    return 0;
}
