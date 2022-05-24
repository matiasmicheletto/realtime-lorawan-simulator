
import { generateRandomString, generateRandomPos } from "./tools";

export default class LoRaWANModel {
    constructor() {
        this.enddevices = [];
        this.gateways = [];
        this.links = [];


        //// TEMP ////
        // Generate random network (2 GW, 40 ED)
        const start = Date.now();
        this.addGateWay(generateRandomPos());
        this.addGateWay(generateRandomPos());
        this.addGateWay(generateRandomPos());
        this.addGateWay(generateRandomPos());
        for(let i = 0; i < 400; i++) 
            this.addEndDevice(generateRandomPos(), 100);
        console.log(`Generated network in ${Date.now() - start} ms`);
    }

    getNodes() {
        return [...this.enddevices, ...this.gateways];
    }

    getEdges() {
        return this.links;
    }

    clearNodes() {
        this.enddevices = [];
    }

    clearEdges() {
        this.links = [];
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
            ...pos,
            period
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