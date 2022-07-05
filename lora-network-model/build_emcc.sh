#!/bin/bash

emcc -lembind -Oz src/libs/random/random.cpp src/libs/random/uniform.cpp src/libs/random/normal.cpp src/libs/random/custom.cpp src/libs/random/clouds.cpp src/libs/network/node.cpp src/libs/network/enddevice.cpp src/libs/network/gateway.cpp src/libs/network/network.cpp src/libs/optimizer/optimizer.cpp src/emcc/jsinterface.cpp src/emcc/main.cpp -o main.js --no-entry
mv main.wasm ../dist-wasm/main.wasm
mv main.js ../dist-wasm/main.js