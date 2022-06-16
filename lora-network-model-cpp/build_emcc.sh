#!/bin/bash

emcc -lembind -Oz src/libs/random/random.cpp src/libs/random/uniform.cpp src/libs/random/normal.cpp src/libs/random/custom.cpp src/libs/random/clouds.cpp src/libs/network/node.cpp src/libs/network/enddevice.cpp src/libs/network/gateway.cpp src/libs/optimizer/networkoptimizer.cpp src/libs/optimizer/optimizerinterface.cpp src/main_emcc.cpp  -o main.js --no-entry