import { sortByClosest } from "../tools/geometry.mjs";
import { lcm } from "../tools/integers.mjs";
import { generateRandomPos, generateRandomString } from "../tools/random.mjs";
import { arrayAvg, arraySum, selectAttributes } from "../tools/structures.mjs";


// List of attributes for exporting nodes and links
export const defaultNodeAttrs = ["id","label","group","x","y","period","sf","connectedTo","UF","channel"];
export const defaultLinkAttrs = ["id","from","to"];

const sfNames = ["NSF","SF7","SF8","SF9","SF10","SF11","SF12","NSF"];


export default class LoRaWANModel {
    constructor() {
        this._hyperperiod = 1; // System hyperperiod
        this._enddevices = []; // List of end devices
        this._gateways = []; // List of gateways
        this._availableChannels = [...Array(16).keys()]; // Each gateway use a different channel
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
                ...pos // Position of the gateway (x,y)
            };
            this._gateways.push(newGateWay);            
            return newGateWay.id;
        } else {
            console.log("Cannot add a new gateway, no more channels available");
            return null;
        }
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
        this._hyperperiod = lcm(this._hyperperiod, period); // Update hyperperiod
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
        if(this._enddevices[edIdx].group === "NCED") {
            this._enddevices[edIdx].connectedTo = this._gateways[gwIdx].id;
            this._enddevices[edIdx].sf = sfNames[sf];
            this._enddevices[edIdx].group = "ED"; // Update group to connected ED
            this._gateways[gwIdx].connectedTo.push(this._enddevices[edIdx].id);
            return true; // Result of connection
        }else{
            console.log("Cannot connect to another gateway", edIdx);
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

    getMinSpreadingFactor(dist) {
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
    }

    getMaxSpreadingFactor(period) {
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
    }

    autoConnect() { // Greedy allocation method
        // Connect end devices to gateways
        for(let gwIdx = 0; gwIdx < this._gateways.length; gwIdx++) { // For each gateway
            const availableSlots = [
                0, // 0 -- No SF for this period (< 100 Hz^-1)
                this._hyperperiod, // 1 -- SF7
                this._hyperperiod/2, // 2 -- SF8
                this._hyperperiod/4, // 3 -- SF9
                this._hyperperiod/8, // 4 -- SF10
                this._hyperperiod/16, // 5 -- SF11
                this._hyperperiod/32, // 6 -- SF12
                0 // 7 -- No SF for this distance (> 2000 mts)
            ];
            const initialSlots = arraySum(availableSlots);
            const {x, y, id:gwId} = this._gateways[gwIdx]; // Gateway position
            // Get all free end devices from closest to farthest in the range of 2000 mts
            const sortedEndDevices = sortByClosest({x, y}, this._enddevices.filter(ed => ed.group === "NCED"), 2000); // Returns distances and indexes
            for(let i = 0; i < sortedEndDevices.length; i++){
                // Get minimal SF for the distance between the end device and the current gateway
                const minSF = this.getMinSpreadingFactor(sortedEndDevices[i].dist);                
                // Limit SF to the end device min frequency
                const maxSF = this.getMaxSpreadingFactor(sortedEndDevices[i].period);
                let sf = minSF; // Greedy method, starts from the min SF
                while(availableSlots[sf] == 0 && sf < maxSF) // If no more available slots for this SF, move to the next
                    sf++;
                if(sf < maxSF){ // If there are available slots, connect the end device to the gateway
                    availableSlots[sf] -= 1;
                    this.connectEndDeviceID(sortedEndDevices[i].id, gwId, sf);
                }
            }
            // Update UF of the gateway
            this._gateways[gwIdx].UF = 1 - arraySum(availableSlots)/initialSlots;
        }
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
        });
    }

    disconnectGateway(gwIdx) { 
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
    }

    refactorGW(mapDimensions, method = "random") { // Improve the allocation of the end devices
        const originalCoverage = this.getNetworkCoverage();
        const originalGWPositions = this._gateways.map(gw => ({x: gw.x, y: gw.y}));
        this.disconnectEndDevices();
        //if(method === "random"){
            this._gateways.forEach(gw => {
                const {x, y} = generateRandomPos(mapDimensions);
                gw.x = x;
                gw.y = y;
            });
        //}
        this.autoConnect();
        const newCoverage = this.getNetworkCoverage();
        if(newCoverage < originalCoverage) { // If the coverage did not improve, revert the changes
            this.disconnectEndDevices();
            this._gateways.forEach((gw, i) => {
                gw.x = originalGWPositions[i].x;
                gw.y = originalGWPositions[i].y;
            });
            this.autoConnect();
        }else{
            console.log("Improvement step:", method, "New coverage:", newCoverage);
        }
    }

    getNetworkCoverage() { // Percentage of end devices connected to gateways
        return this._enddevices.filter(ed => ed.group === "ED").length / this._enddevices.length*100;
    }

    getNetworkStats() {
        const stats = {
            ufAvg: arrayAvg(this._gateways.map(gw => gw.UF)),
            coverage: this.getNetworkCoverage()
        };
        return stats;
    }
};