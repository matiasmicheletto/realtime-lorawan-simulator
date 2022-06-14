#include <emscripten/bind.h>
#include "networkoptimizerrandom.h"
#include "networkoptimizersprings.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(my_module) {
    class_<NetworkOptimizerRandom>("NetworkOptimizerRandom")
        .constructor<>()
        //.function("init", &NetworkOptimizerRandom::init)
        .function("minimizeGW", &NetworkOptimizerRandom::minimizeGW)
        .function("printStatus", &NetworkOptimizerRandom::printStatus)
    ;
}