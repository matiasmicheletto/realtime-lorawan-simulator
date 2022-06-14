#!/bin/bash

emcc -lembind tools.cpp node.cpp enddevice.cpp gateway.cpp networkoptimizer.cpp networkoptimizerrandom.cpp networkoptimizersprings.cpp main_emcc.cpp -o main.js --no-entry