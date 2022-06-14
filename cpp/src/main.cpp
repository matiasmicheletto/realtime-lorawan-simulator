#include "libs/optimizer/networkoptimizersprings.h"
#include "libs/random/uniform.h"
//#include "libs/random/normal.h"
#include "libs/random/custom.h"
#include <stdio.h>

int main(int argc, char **argv) {
    // Number of end devices
    const unsigned int N = 6500; 
    // Size of the map (assumed to be square)
    const unsigned int map = 1000; 
    // Function to generate the positions of the end devices
    Uniform posGenerator = Uniform(-(double)map/2, (double)map/2); // Uniform
    //Normal posGenerator = Normal(0, 0.15); // Normal distribution with center in 0,0
    
    // Function to generate the periods of the end devices
    CustomDist periodGenerator = CustomDist::Builder()
        .addValue(3600, 0.97)
        ->addValue(1800, 0.01)
        ->addValue(900, 0.02)
        ->build();
    
    NetworkOptimizerSprings* optimizer = new NetworkOptimizerSprings();

    optimizer->init(N, map, &posGenerator, &periodGenerator);
    optimizer->minimizeGW(100, 60);
    optimizer->printStatus();

    delete optimizer;
    return 0;
}
