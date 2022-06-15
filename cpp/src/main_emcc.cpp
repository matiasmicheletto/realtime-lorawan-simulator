#include <emscripten/bind.h>
#include "libs/optimizer/networkoptimizer.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(my_module) {
    class_<NetworkOptimizer>("NetworkOptimizer")
        .constructor<>()
        .function("run", &NetworkOptimizer::run)
        .function("printStatus", &NetworkOptimizer::printStatus)
        ;
}