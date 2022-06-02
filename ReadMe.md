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
```js
import Manager from '/lora-network-model/model-manager';
const parameters = {
    N: 500,
    H: 3600,
    mapWidth: 1000, 
    mapHeight: 1000,
    posDistr: "uniform",
    periodsDistr: "97, 1, 0, 2", // 97% -> 3600, 1% -> 1800, 0% -> 1200, 2% -> 900
    initialGW: 2,
    strategy: "random",
    maxIter: 100,
    maxRuntime: 60
};
const manager = new Manager(parameters);
manager.initialize();
manager.run().then(results => console.log(results))
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

