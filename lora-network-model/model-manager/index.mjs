import { normalDist, uniformDist, generateRandomPos } from "../tools/random.mjs";
import { arrayAvg } from "../tools/structures.mjs";
import { createEDNetwork } from "../instance-generator/index.mjs";
import LoRaWANModel from "../network-model/index.mjs";

const posDistrFc = {
    uniform: uniformDist,
    normal: normalDist
};

const defaultParameters = {
    N: 10000,
    H: 3600,
    mapWidth: 1000, 
    mapHeight: 1000,
    posDistr: "uniform",
    periodsDistr: "97, 1, 0, 2", // 97% -> 3600, 1% -> 1800, 0% -> 1200, 2% -> 900
    initialGW: 5,
    strategy: "random",
    maxIter: 1000,
    maxRuntime: 60,
    updateFreq: 1
};

export default class Manager {
    constructor() {
        this.configure(defaultParameters);
        this.model = null;
        this.ready = false;
    }

    configure(params) {
        Object.assign(this, params);
    }

    getParam(param) {
        return this[param];
    }

    getAllParams() {
        return {
            N: this.N,
            H: this.H,
            mapWidth: this.mapWidth,
            mapHeight: this.mapHeight,
            posDistr: this.posDistr,
            periodsDistr: this.periodsDistr,
            initialGW: this.initialGW,
            strategy: this.strategy,
            maxIter: this.maxIter,
            maxRuntime: this.maxRuntime,
            updateFreq: this.updateFreq
        };
    }

    getAllNodes(attrs) {
        return this.model.getAllNodes(attrs);
    }

    getLinks(attrs) {
        return this.model.getLinks(attrs);
    }

    initialize() {
        const instance = createEDNetwork(
            this.N, 
            this.H, 
            posDistrFc[this.posDistr], 
            [this.mapWidth, this.mapHeight], 
            this.periodsDistr.split(",").map(p => parseInt(p))
        ); 

        this.model = new LoRaWANModel();
        instance.forEach(ed => {
            this.model.addEndDevice(ed.position, ed.period);
        });

        for(let i = 0; i < this.initialGW; i++) 
        this.model.addGateway(generateRandomPos([this.mapWidth, this.mapHeight]));        

        const start = Date.now();
        this.model.autoConnect();
        const elapsed = Date.now() - start;
        console.log(`Elapsed time: ${elapsed}ms`);

        this.ready = true;
    }

    getNetworkStats() {
        const stats = {
            ufAvg: arrayAvg(this.model.getGateways().map(gw => gw.UF)),
            ncEdsCnt: this.model.getEndDevices().filter(ed => ed.group === "NCED").length
        };
        return stats;
    }

    run() {
        return new Promise(resolve => {
            setTimeout(() => {
                const start = Date.now();
                for(let i = 0; i < this.maxIter; i++) {
                    this.model.disconnectEndDevices();
                    const idx = 0;
                    const position = generateRandomPos([this.mapWidth, this.mapHeight]);
                    this.model.moveGatewayIdx(idx, position);
                    this.model.autoConnect();
                    if((Date.now() - start)/1000 > this.maxRuntime) break;
                }
        
                const results = {
                    ...this.getNetworkStats(),
                    elapsed: Date.now() - start
                };
                resolve(results);
            }, 10);
        });
    }
}