#!/bin/bash

#emcc -sSTANDALONE_WASM -o main.wasm --no-entry \
emcc -Oz -o main.js --no-entry -sEXPORTED_FUNCTIONS=_free,_runSimulation -sEXPORTED_RUNTIME_METHODS=ccall,cwrap \
src/libs/random/random.cpp \
src/libs/random/uniform.cpp \
src/libs/random/normal.cpp \
src/libs/random/custom.cpp \
src/libs/random/clouds.cpp \
src/libs/network/node.cpp \
src/libs/network/enddevice.cpp \
src/libs/network/gateway.cpp \
src/libs/network/network.cpp \
src/libs/optimizer/optimizer.cpp \
src/main_emcc.cpp

cp main.wasm ../public/main.wasm
if [ -f "../public/main.js" ]; then
    cp main.js ../public/main.js
fi
