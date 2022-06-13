import { 
    generateRandomDistribution,
    generateRandomPos,
    randomSelect,
    uniformDist, 
    generateRandomString
} from '../tools/random.mjs';
import { getAllDivisors, clamp } from '../tools/integers.mjs';
import { getClosest, sortByClosest } from "../tools/geometry.mjs";
import { arrayAvg, arraySum, selectAttributes } from "../tools/structures.mjs";


// List of attributes for exporting nodes and links
export const defaultNodeAttrs = ["id","label","group","x","y","period","sf","connectedTo","UF","channel"];

const sfNames = ["NSF","SF7","SF8","SF9","SF10","SF11","SF12","NSF"];

// Initial available slots for a gateway
const getSlots = H => [ 
    0, // 0 -- No SF for this period (< 100 Hz^-1)
    H, // 1 -- SF7
    H/2, // 2 -- SF8
    H/4, // 3 -- SF9
    H/8, // 4 -- SF10
    H/16, // 5 -- SF11
    H/32, // 6 -- SF12
    0 // 7 -- No SF for this distance (> 2000 mts)
];

const getMinSpreadingFactor = dist => {
    if(dist < 62.5)
        return 1; // SF7
    else if(dist < 125)
        return 2; // SF8
    else if(dist < 250)
        return 3; // SF9
    else if(dist < 500)
        return 4; // SF10
    else if(dist < 1000)
        return 5; // SF11
    else if(dist < 2000)
        return 6; // SF12
    else // No SF for this distance
        return 7;
};

const getMaxSpreadingFactor = period => {
    if(period > 3200)
        return 6; // SF12
    else if(period > 1600)
        return 5; // SF11
    else if(period > 800)
        return 4; // SF10
    else if(period > 400)
        return 3; // SF9
    else if(period > 200)
        return 2; // SF8
    else if(period > 100)
        return 1; // SF7
    else // No SF for this period
        return 0;
};

export default class LoRaWANModel {
    constructor(edNumber, hyperperiod, posDistr = uniformDist, mapSize = [100, 100], periodsDistr = [90, 10]) {
        this._hyperperiod = hyperperiod; // System hyperperiod
        this._initialSlots = arraySum(getSlots(this._hyperperiod)); // Initial available slots for a gateway

        this._mapSize = mapSize; // Size of the map

        this._gateways = []; // List of gateways
        this._availableChannels = [...Array(16).keys()]; // Each gateway use a different channel

        // Generate end devices
        this._enddevices = []; // List of end devices
        const divisors = getAllDivisors(hyperperiod);
        const periodsDist = generateRandomDistribution(divisors, periodsDistr);
        for(let i = 0; i < edNumber; i++) 
            this.addEndDevice(generateRandomPos(this._mapSize, posDistr), randomSelect(periodsDist));

        // Set default strategies
        this.setAutoConnectMethod("gw");
        this.setRefactorStepMethod("random");

        this._covDiff = 0; // Coverage difference on gw positions refactoring
    }

    get edNumber() {
        return this._enddevices.length;
    }

    getEndDevices(attrs = defaultNodeAttrs) {
        return selectAttributes(this._enddevices, attrs);
    }

    getGateways(attrs = defaultNodeAttrs) {
        return selectAttributes(this._gateways, attrs);
    }

    getAllNodes(attrs = defaultNodeAttrs) {
        return selectAttributes([...this._gateways, ...this._enddevices], attrs);
    }

    addGateway(pos) {        
        if(this._availableChannels.length > 0) {
            const newGateWay = {
                id: generateRandomString(), // Unique identifier
                label: `Gateway ${this._gateways.length+1}`, // Label for visualization
                group: "GW", // Group for visualization (Gateway)
                connectedTo: [], // List of connected end devices
                UF: 0, // Utilization factor
                channel: this._availableChannels.shift(), // Channel used by the gateway
                availableSlots: getSlots(this._hyperperiod), // Available slots for each spreading factor
                ...pos // Position of the gateway (x,y)
            };
            this._gateways.push(newGateWay);            
            return newGateWay.id;
        } else {
            console.log("Cannot add a new gateway, no more channels available");
            return null;
        }
    }

    setAutoConnectMethod(method) {
        if(method === "gw")
            this.autoConnect = this._autoConnectGW;
        if(method === "ed")
            this.autoConnect = this._autoConnectED;
    }

    setRefactorStepMethod(method) {
        if(method === "random")
            this.refactorStep = this._randomizeGW;
        if(method === "spring")
            this.refactorStep = this._springLayout;
    }

    moveGatewayIdx(gwIdx, pos) {
        if(this._gateways[gwIdx].connectedTo.length === 0){
            this._gateways[gwIdx].x = pos.x;
            this._gateways[gwIdx].y = pos.y;
        }else{
            console.log("Cannot move a gateway with connected end devices");
        }
    }

    moveGatewayId(gwId, pos) {
        const gwIdx = this._gateways.findIndex(el => el.id === gwId);
        if(gwIdx >= 0)
            return this.moveGatewayIdx(gwIdx, pos);
        return null;
    }

    addEndDevice(pos, period) {
        const newEndDevice = {
            id: generateRandomString(), // Unique identifier
            label: `End device ${this._enddevices.length+1}`, // Label for visualization
            group: "NCED", // Group for visualization (Not connected end device)
            connectedTo: null, // Id of connected gateway
            sf: null, // Spreading factor
            period, // Transmission period of the end device
            ...pos, // Position of the end device (x,y)
        };
        this._enddevices.push(newEndDevice);
        return newEndDevice.id;
    }

    connectEndDeviceIdx(edIdx, gwIdx, sf) {
        if(this._enddevices[edIdx].group === "NCED" && this._gateways[gwIdx].availableSlots[sf] > 0) {
            this._enddevices[edIdx].connectedTo = this._gateways[gwIdx].id;
            this._enddevices[edIdx].sf = sfNames[sf];
            this._enddevices[edIdx].group = "ED"; // Update group to connected ED
            this._gateways[gwIdx].connectedTo.push(this._enddevices[edIdx].id);
            //this._gateways[gwIdx].availableSlots[sf] -= 1;
            this._gateways[gwIdx].availableSlots[sf] -= this._hyperperiod / this._enddevices[edIdx].period;
            return true; // Result of connection
        }else{
            //console.log("Cannot connect to gateway", edIdx, gwIdx, sf);
            return false; // Result of connection
        }
    }

    connectEndDeviceID(edId, gwId, sf) {
        const edIdx = this._enddevices.findIndex(el => el.id === edId);
        const gwIdx = this._gateways.findIndex(el => el.id === gwId);
        if(edIdx >= 0 && gwIdx >= 0)
            return this.connectEndDeviceIdx(edIdx, gwIdx, sf);
        return null;
    }

    _autoConnectGW() { // Connect end devices to gateways
        // Greedy allocation method, scheduling by gateway
        for(let gwIdx = 0; gwIdx < this._gateways.length; gwIdx++) { // For each gateway
            const {x, y, id:gwId} = this._gateways[gwIdx]; // Gateway position
            // Get all free end devices from closest to farthest in the range of 2000 mts
            const sortedEndDevices = sortByClosest({x, y}, this._enddevices.filter(ed => ed.group === "NCED"), 2000); // Returns distances and indexes
            for(let edIdx = 0; edIdx < sortedEndDevices.length; edIdx++){
                // Get minimal SF for the distance between the end device and the current gateway
                const minSF = getMinSpreadingFactor(sortedEndDevices[edIdx].dist);                
                // Limit SF to the end device min frequency
                const maxSF = getMaxSpreadingFactor(sortedEndDevices[edIdx].period);
                let sf = minSF; // Greedy method, starts from the min SF
                while(!this.connectEndDeviceID(sortedEndDevices[edIdx].id, gwId, sf) && sf < maxSF) // Try to connect with the lower SF
                    sf++;
            }
            // Update UF of the gateway
            this._gateways[gwIdx].UF = 1 - arraySum(this._gateways[gwIdx].availableSlots)/this._initialSlots;
        }
    }

    _autoConnectED() { // Connect end devices to gateways
        // Greedy allocation method, scheduling by end device
        for(let edIdx = 0; edIdx < this._enddevices.length; edIdx++) { // For each end device
            const {x, y, id:edId} = this._enddevices[edIdx]; // End device positions
            const sortedGateways = sortByClosest({x, y}, this._gateways, 2000); // Returns distances and indexes
            for(let gwIdx = 0; gwIdx < sortedGateways.length; gwIdx++) {
                // Get minimal SF for the distance between the end device and the current gateway
                const minSF = getMinSpreadingFactor(sortedGateways[gwIdx].dist);
                // Limit SF to the end device min frequency
                const maxSF = getMaxSpreadingFactor(this._enddevices[edIdx].period);
                let sf = minSF; // Greedy method, starts from the min SF
                while(!this.connectEndDeviceID(edId, sortedGateways[gwIdx].id, sf) && sf < maxSF) // Try to connect with the lower SF
                    sf++;
                if(this._enddevices[edIdx].group === "ED") // If end device was connected, continue with the next end device
                    break;
            }
        }
        // Update UF of the gateways
        this._gateways.forEach(gw => {
            gw.UF = 1 - arraySum(gw.availableSlots)/this._initialSlots;
        });
    }

    disconnectEndDevices() { // Delete all connectons
        this._enddevices.forEach(ed => {
            ed.connectedTo = null;
            ed.sf = null;
            ed.group = "NCED";
        });
        this._gateways.forEach(gw => {
            gw.connectedTo = [];
            gw.UF = 0;
            gw.availableSlots = getSlots(this._hyperperiod);
        });
    }

    disconnectGateway(gwIdx, remove = false) { 
        // Delete all connections of a single gateway. Disconnected end devices
        // are not automatically connected to another gateway, unless autoConnect
        // method is called after removing the current gateway.
        const gw = this._gateways[gwIdx];
        gw.connectedTo.forEach(edId => {
            const edIdx = this._enddevices.findIndex(el => el.id === edId);
            if(edIdx >= 0) {
                this._enddevices[edIdx].connectedTo = null;
                this._enddevices[edIdx].sf = null;
                this._enddevices[edIdx].group = "NCED";
            }            
        });
        gw.connectedTo = [];
        gw.UF = 0;
        this._availableChannels.push(gw.channel)
        if(remove)
            this._gateways.splice(gwIdx, 1);
    }

    _randomizeGW() { // Try to improve the network coverage by setting a new random position for each gateway
        const originalCoverage = this.getNetworkCoverage();
        const originalGWPositions = this._gateways.map(gw => ({x: gw.x, y: gw.y}));
        this.disconnectEndDevices();
        this._gateways.forEach(gw => {
            const {x, y} = generateRandomPos(this._mapSize);
            gw.x = x;
            gw.y = y;
        });
        this.autoConnect();
        const newCoverage = this.getNetworkCoverage();
        if(newCoverage < originalCoverage) { // If the coverage did not improve, revert the changes
            this.disconnectEndDevices();
            this._gateways.forEach((gw, idx) => {
                gw.x = originalGWPositions[idx].x;
                gw.y = originalGWPositions[idx].y;
            });
            this.autoConnect();
        }
        this._covDiff = newCoverage - originalCoverage;
        //console.log("Coverage difference:", this._covDiff);
    }

    _springLayout() { // Try to improve the network coverage by setting a new random position for each gateway
        const originalCoverage = this.getNetworkCoverage();
        const forces = [];
        const nced = this._enddevices.filter(ed => ed.group === "NCED");
        const N = nced.length;
        for(let i = 0; i < nced.length; i++) {
            const closest = getClosest(nced[i], this._gateways);
            const gw = this._gateways[closest.idx];
            forces.push({
                gwId: gw.id,
                x: (nced[i].x - gw.x)/N,
                y: (nced[i].y - gw.y)/N
            });
        }
        this.disconnectEndDevices();
        this._gateways.forEach(gw => {
            const gwForces = forces.filter(f => f.gwId === gw.id);
            const xForce = arraySum(gwForces.map(f => f.x));
            const yForce = arraySum(gwForces.map(f => f.y));
            gw.x += clamp(xForce, -100, 100);
            gw.y += clamp(yForce, -100, 100);
        });
        this.autoConnect();
        const newCoverage = this.getNetworkCoverage();
        this._covDiff = newCoverage - originalCoverage;
        //console.log("Coverage difference:", this._covDiff);
    }

    getNetworkCoverage() { // Percentage of end devices connected to gateways
        return this._enddevices.filter(ed => ed.group === "ED").length / this._enddevices.length*100;
    }

    getAverageUF() { // Average UF of the gateways
        return arrayAvg(this._gateways.map(gw => gw.UF));
    }

    getNetworkStats() { // All stats of the network
        const stats = {
            ufAvg: this.getAverageUF(),
            coverage: this.getNetworkCoverage(),
            coverageDiff: this._covDiff,
            //edNum: this._enddevices.length,
            gwNum: this._gateways.length
        };
        return stats;
    }
};