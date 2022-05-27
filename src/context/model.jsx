import React, {createContext} from 'react';
import LoRaWANModel from 'lora-network-model/network-model/index.mjs';
import { createEDNetwork } from 'lora-network-model/instance-generator/index.mjs';
import { uniformDist } from 'lora-network-model/tools/random.mjs';

// Parameters
const N = 500; // Number of end devices (real time periodic messages)
const mapSize = [1000, 1000]; // Size of the map
const H = 3600; // Hyperperiod of the system
const pDist = [97, 1, 0, 2]; // Period distribution of the end devices

// Generate random positions and periods
const instance = createEDNetwork(N, H, uniformDist, mapSize, pDist); 

// Create network model with generated end devices
const model = new LoRaWANModel();
instance.forEach(ed => {
    model.addEndDevice(ed.position, ed.period);
});

// Add gateways
model.addGateway({x:300, y:0});
model.addGateway({x:-300, y:0});

// Connect end devices to gateways
console.time('Scheduling time');
model.autoConnect();
console.timeEnd('Scheduling time');

export const ModelCtx = createContext();

export const ModelProvider = props => (
    <ModelCtx.Provider value={model}>
        {props.children}
    </ModelCtx.Provider>
);