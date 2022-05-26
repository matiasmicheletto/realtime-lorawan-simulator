# Real-time LoRaWAN Simulator

A tool for generate, analyze and visualize city-wide, real-time LoRaWAN networks composed of a few thousand to millions end devices. 

![model](doc/systemmodel.png)

![screenshot](doc/screenshot.png)

# Getting started

## Installation
```bash
git clone https://github.com/matiasmicheletto/realtime-lorawan-simulator
cd realtime-lorawan-simulator
npm install
```


## JS scripting tool
The library can be used with NodeJs. The [examples](lora-network-model/examples) folder contain different demonstrations.
Minimal example to test how many disconnected nodes are in a network with 10 end devices and a single Gateway in the center of the map:
```js
import LoRAWANModel from './network-model/index.mjs';
import { createEDNetwork } from './instance-generator/index.mjs';

const N = 10; // Number of end devices (real time periodic messages)
const H = 1000; // Hyperperiod of the system
const instance = createEDNetwork(N, H); // Generate random positions and periods

// Create model with end devices
const model = new LoRAWANModel();
instance.forEach(node => {
    model.addEndDevice(node.position, node.period);
});

model.addGateWay({x:0, y:0});
model.autoConnect(); // TODO: implement different policies

// Count not connected end devices (non feasible messages)
const nonFeasibles = model.getAllNodes().filter(node => !node.connected);
console.log("Not feasibles count: ",nonFeasibles.length);
```

## GUI

### Use live (development) version
```bash
npm run dev
```

### Compile optimized version
```bash
npm run build
serve dist
```

