
import { generateRandomString } from "../tools/random.mjs";
import { selectAttributes } from "../tools/structures.mjs";


// List of attributes for exporting nodes and links
export const defaultNodeAttrs = ["id","label","group","x","y","period"];
export const defaultLinkAttrs = ["id","from","to"];

export const visNodeAttrs = ["id","label","group","x","y"];
export const visLinkAttrs = ["id","from","to"];

export const exportNodeAttrs = ["x", "y", "period"];
export const exportLinkAttrs = ["from","to"];

export default class LoRaWANModel {
    constructor() {
        this.enddevices = [];
        this.gateways = [];
        this.links = [];
    }

    getEndDevices(attrs = defaultNodeAttrs) {
        return selectAttributes(this.enddevices, attrs);
    }

    getGateWays(attrs = defaultNodeAttrs) {
        return selectAttributes(this.gateways, attrs);
    }

    getAllNodes(attrs = defaultNodeAttrs) {
        return selectAttributes([...this.gateways, ...this.enddevices], attrs);
    }

    getLinks(attrs = defaultLinkAttrs) {
        return selectAttributes(this.links, attrs);
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

    getClosestGatewayId(pos) {
        let minDist = Number.MAX_SAFE_INTEGER;
        let minGateway = null;        
        for(let i = 0; i < this.gateways.length; i++) {
            const dist = Math.sqrt(Math.pow(pos.x - this.gateways[i].x, 2) + Math.pow(pos.y - this.gateways[i].y, 2));            
            if(dist < minDist) {
                minDist = dist;
                minGateway = this.gateways[i];
            }
        }
        return minGateway.id;
    }

    getRandomGatewayId() {        
        const idx = Math.floor(Math.random() * this.gateways.length);
        return this.gateways[idx].id;
    }

    addGateWay(pos) {
        const id = generateRandomString();
        const newGateWay = {
            id,
            label: `Gateway ${this.gateways.length+1}`,
            group: "GW",
            period: null,
            ...pos
        };
        this.gateways.push(newGateWay);
        return newGateWay.id;
    }

    addEndDevice(pos, period) {
        const newEndDevice = {
            id: generateRandomString(),
            label: `End device ${this.enddevices.length+1}`,
            group: "ED",
            connected: null,
            period,
            ...pos,
        };
        this.enddevices.push(newEndDevice);
        return newEndDevice.id;
    }

    connectEndDeviceID(edId, gwId) {
        const edIdx = this.enddevices.findIndex(el => el.id === edId);
        const gwIdx = this.gateways.findIndex(el => el.id === gwId);
        if(edIdx >= 0 && gwIdx >= 0){
            this.enddevices[edIdx].connected = gwId;
            const newLink = {
                id: generateRandomString(),
                from: edId,
                to: gwId
            };
            this.links.push(newLink);
            return newLink.id;
        } 
        return null;
    }

    connectEndDeviceIdx(edIdx, gwIdx) {
        this.enddevices[edIdx].connected = this.gateways[gwIdx].id;
        const newLink = {
            id: generateRandomString(),
            from: this.enddevices[edIdx].id,
            to: this.gateways[gwIdx].id
        };
        this.links.push(newLink);
        return newLink.id;
    }

    autoConnect() {
        
    }
};