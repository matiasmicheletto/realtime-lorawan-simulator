import LoRaWANModel from '../network-model/index.mjs';
import { createEDNetwork } from '../instance-generator/index.mjs';
import { uniformDist } from '../tools/random.mjs';

// Parameters
const N = 6500; // Number of end devices (real time periodic messages)
const mapSize = [100, 100]; // Size of the map
const H = 3600; // Hyperperiod of the system
const pDist = [97, 1, 0, 2]; // Period distribution of the end devices

// Generate random positions and periods
const instance = createEDNetwork(N, H, uniformDist, mapSize, pDist); 

// Create network model with generated end devices
const model = new LoRaWANModel();
instance.forEach(ed => {
    model.addEndDevice(ed.position, ed.period);
});

// Add a single gateway
model.addGateway({x:0, y:0});

// Connect end devices to gateways
console.time('Scheduling time');
model.autoConnect();
console.timeEnd('Scheduling time');

// Count not connected end devices (non feasible messages)
const nonFeasibles = model.getEndDevices().filter(node => node.group === "NCED");

//console.log(model._enddevices);
console.log("Not feasibles count:", nonFeasibles.length, `${(nonFeasibles.length/N*100).toFixed(2)} %`);