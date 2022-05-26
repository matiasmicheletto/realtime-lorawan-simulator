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