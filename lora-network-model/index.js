
import { 
    generateRandomString, 
    generateRandomPos, 
    getRandomDivisor,
    selectAttributes
 } from "./tools";

export const defaultNodeAttrs = ["id","label","group","x","y","period"];
export const defaultLinkAttrs = ["id","from","to"];

export const visNodeAttrs = ["id","label","group","x","y"];
export const visLinkAttrs = ["id","from","to"];

export const exportNodeAttrs = ["x", "y", "period"];
export const exportLinkAttrs = ["from","to"];

export default class LoRaWANModel {
    constructor(edNumber, hyperperiod) {
        this.init(edNumber, hyperperiod);
    }

    init(edNumber = 50, hyperperiod = 3600) {
        this.enddevices = [];
        this.gateways = [];
        this.links = [];
        this.edNumber = edNumber;
        this.hyperperiod = hyperperiod;

        //// TEMP ////        
        const start = Date.now();
        this.addGateWay(generateRandomPos());
        this.addGateWay(generateRandomPos());
        this.addGateWay(generateRandomPos());
        this.addGateWay(generateRandomPos());
        for(let i = 0; i < this.edNumber; i++)
            this.addEndDevice(generateRandomPos(), getRandomDivisor(this.hyperperiod));
        console.log(`Generated network in ${Date.now() - start} ms`);
        console.log(this.enddevices);
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

    exportModel() {
        return JSON.stringify({
            enddevices: this.getEndDevices(exportNodeAttrs),
            gateways: this.getGateWays(exportNodeAttrs),
            links: this.getLinks(exportLinkAttrs)
        });
    }

    resetModel(edNumber = 50, hyperperiod = 3600) {
        this.enddevices = [];
        this.gateways = [];
        this.links = [];
        this.edNumber = edNumber;
        this.hyperperiod = hyperperiod;
        this.init();
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
            period,
            ...pos,
        };
        const link = {
            id: generateRandomString(),
            from: newEndDevice.id,
            to: this.getClosestGatewayId(pos)
        };
        this.enddevices.push(newEndDevice);
        this.links.push(link);
        return {edId: newEndDevice.id, linkId: link.id};
    }
};