#include <emscripten.h>
#include <emscripten/bind.h>
#include "libs/optimizer/optimizerinterface.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(optimizer_interface) {
    class_<OptimizerInterface>("OptimizerInterface")
        .constructor<unsigned int, unsigned int, unsigned int, int, int, int>()
        .function("run", &OptimizerInterface::run)
    ;
}
