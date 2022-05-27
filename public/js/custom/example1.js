// Parameters
const N = 500; // Number of end devices (real time periodic messages)
const mapSize = [1000, 1000]; // Size of the map
const H = 3600; // Hyperperiod of the system
const pDist = [97, 1, 0, 2]; // Period distribution of the end devices

// Generate random positions and periods
const instance = createEDNetwork(N, H, uniformDist, mapSize, pDist); 

// Create network model with generated end devices
const model = new LoRaWANModel();
instance.forEach(ed => { // Add each end device to the network model
    model.addEndDevice(ed.position, ed.period);
});

// Add a pair of gateways
model.addGateway({x:-300, y:100});
model.addGateway({x:300, y:-100});


// Connect end devices to gateways
console.time('Scheduling time');
model.autoConnect();
console.timeEnd('Scheduling time');

// Count not connected end devices (non feasible messages)
const nonFeasibles = model.getEndDevices().filter(node => node.group === "NCED");
const result = `<br>Not feasibles count: ${nonFeasibles.length} (${(nonFeasibles.length/N*100).toFixed(2)}) %<br>`;
document.write(result);
console.log(result);
console.table(nonFeasibles);

const gws = model.getGateways();
gws.forEach(gw => {
    document.write(`<br>Gateway ${gw.id} has ${gw.connectedTo.length} end devices<br>`);
});