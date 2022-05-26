import LoRAWANModel from '../network-model/index.mjs';
import { createEDNetwork } from '../instance-generator/index.mjs';

const N = 10000; // Number of end devices (real time periodic messages)
const H = 3200; // Hyperperiod of the system
const instance = createEDNetwork(N, H); // Generate random positions and periods

// Create model with end devices
const model = new LoRAWANModel();
instance.forEach(node => {
    model.addEndDevice(node.position, node.period);
});

model.addGateway({x:0, y:0});
console.time('connect');
model.autoConnect();
console.timeEnd('connect');

// Count not connected end devices (non feasible messages)
const nonFeasibles = model.getEndDevices().filter(node => node.connected === null);
//console.log(model._enddevices);
console.log("Not feasibles count: ",nonFeasibles.length);