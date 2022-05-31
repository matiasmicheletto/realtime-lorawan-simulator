// List of attributes for exporting nodes and links
const defaultNodeAttrs = ["id","label","group","x","y","period","sf","connectedTo"];
const defaultLinkAttrs = ["id","from","to"];

//const visNodeAttrs = ["id","label","group","x","y"];
const visNodeAttrs = ["id","group","x","y"];
const visLinkAttrs = ["id","from","to"];

const exportNodeAttrs = ["x", "y", "period"];
const exportLinkAttrs = ["from","to"];

class LoRaWANModel {
    constructor() {
        this._hyperperiod = 1; // System hyperperiod
        this._enddevices = []; // List of end devices
        this._gateways = []; // List of gateways
        this._links = []; // List of connection from end devices to gateways
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

    getLinks(attrs = defaultLinkAttrs) {
        return selectAttributes(this._links, attrs);
    }

    exportModel(format) {
        switch(format){
            case "json":
                return JSON.stringify({
                    enddevices: this.getEndDevices(exportNodeAttrs),
                    gateways: this.getGateways(exportNodeAttrs),
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
            group: "GW", // Group for visualization (Gateway)
            connectedTo: [], // List of connected end devices
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
            this._enddevices[edIdx].sf = sf;
            this._enddevices[edIdx].group = "ED"; // Update group to connected ED
            this._gateways[gwIdx].connectedTo.push(this._enddevices[edIdx].id);
            const newLink = {
                id: generateRandomString(),
                from: this._enddevices[edIdx].id,
                to: this._gateways[gwIdx].id,
                sf
            };
            this._links.push(newLink);
            return newLink.id;
        }else{
            console.log("Cannot connect to another gateway");
            return null;
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
        }
    }
};