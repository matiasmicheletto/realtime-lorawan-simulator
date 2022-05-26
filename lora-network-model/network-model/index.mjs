
import { sortByClosest } from "../tools/geometry.mjs";
import { lcm } from "../tools/integers.mjs";
import { generateRandomString } from "../tools/random.mjs";
import { selectAttributes } from "../tools/structures.mjs";


// List of attributes for exporting nodes and links
export const defaultNodeAttrs = ["id","label","group","x","y","period","sf","connected"];
export const defaultLinkAttrs = ["id","from","to"];

export const visNodeAttrs = ["id","label","group","x","y"];
export const visLinkAttrs = ["id","from","to"];

export const exportNodeAttrs = ["x", "y", "period"];
export const exportLinkAttrs = ["from","to"];

export default class LoRaWANModel {
    constructor() {
        this._hyperperiod = 1; // System hyperperiod
        this._enddevices = []; // List of end devices
        this._gateways = []; // List of gateways
        this._links = []; // List of connection from end devices to gateways
    }

    getEndDevices(attrs = defaultNodeAttrs) {
        return selectAttributes(this._enddevices, attrs);
    }

    getGateWays(attrs = defaultNodeAttrs) {
        return selectAttributes(this._gateways, attrs);
    }

    getAllNodes(attrs = defaultNodeAttrs) {
        return selectAttributes([...this._gateways, ...this._enddevices], attrs);
    }

    getLinks(attrs = defaultLinkAttrs) {
        return selectAttributes(this._links, attrs);
    }

    exportModel(format) {
        switch(format){
            case "json":
                return JSON.stringify({
                    enddevices: this.getEndDevices(exportNodeAttrs),
                    gateways: this.getGateWays(exportNodeAttrs),
                    links: this.getLinks(exportLinkAttrs)
                });
            case "csv":
                return null; // TODO
            case "matlab": // TODO
                return null;
            default: 
                return null;
        }
    }

    addGateway(pos) {        
        const newGateWay = {
            id: generateRandomString(), // Unique identifier
            label: `Gateway ${this._gateways.length+1}`, // Label for visualization
            group: "GW", // Group for visualization
            ...pos // Position of the gateway (x,y)
        };
        this._gateways.push(newGateWay);
        return newGateWay.id;
    }

    addEndDevice(pos, period) {
        this._hyperperiod = lcm(this._hyperperiod, period); // Update hyperperiod
        const newEndDevice = {
            id: generateRandomString(), // Unique identifier
            label: `End device ${this._enddevices.length+1}`, // Label for visualization
            group: "ED", // Group for visualization
            connected: null, // Id of connected gateway
            sf: null, // Spreading factor
            period, // Transmission period of the end device
            ...pos, // Position of the end device (x,y)
        };
        this._enddevices.push(newEndDevice);
        return newEndDevice.id;
    }

    connectEndDeviceIdx(edIdx, gwIdx, sf) {
        this._enddevices[edIdx].connected = this._gateways[gwIdx].id;
        this._enddevices[edIdx].sf = sf;
        const newLink = {
            id: generateRandomString(),
            from: this._enddevices[edIdx].id,
            to: this._gateways[gwIdx].id,
            sf
        };
        this._links.push(newLink);
        return newLink.id;
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
            return 1;
        else if(dist < 125)
            return 2;
        else if(dist < 250)
            return 3;
        else if(dist < 500)
            return 4;
        else if(dist < 1000)
            return 5;
        else if(dist < 2000)
            return 6;
        else // No SF for this distance
            return 7;
    }

    getMaxSpreadingFactor(period) {
        if(period > 3200)
            return 6;
        else if(period > 1600)
            return 5;
        else if(period > 800)
            return 4;
        else if(period > 400)
            return 3;
        else if(period > 200)
            return 2;
        else if(period > 100)
            return 1;
        else // No SF for this period
            return 0;
    }

    autoConnect() { // Greedy allocation method
        // Connect end devices to gateways
        this._gateways.forEach((gw, gwIdx) => { // For each gateway, connect all devices in range
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
            const {x, y} = gw; // Gateway position
            // Get all end devices from closest to farthest in the range of 2000 mts
            const endDevices = sortByClosest({x, y}, this._enddevices, 2000); 
            for(let i = 0; i < endDevices.length; i++){
                // Get minimal SF for the distance between the end device and the current gateway
                const minSF = this.getMinSpreadingFactor(endDevices[i].dist);                
                // Limit SF to the end device min frequency
                const maxSF = this.getMaxSpreadingFactor(this._enddevices[endDevices[i].idx].period);
                let sf = minSF;                              
                while(availableSlots[sf] == 0 && sf < maxSF)
                    sf++;
                // If there are available slots, connect the end device to the gateway
                if(sf < maxSF){
                    availableSlots[sf] -= 1;                    
                    this.connectEndDeviceIdx(endDevices[i].idx, gwIdx, sf);
                }
            }
        })
    }
};