#include <emscripten.h>
#include <emscripten/bind.h>
#include "jsinterface.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(js_interface) {
    class_<JsInterface>("JsInterface")
        .constructor<unsigned int, unsigned int, unsigned char, unsigned char, unsigned char, unsigned int, unsigned int, unsigned char, unsigned int>()
        .function("run", &JsInterface::run)
    ;
}
