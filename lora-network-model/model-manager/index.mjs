import { generateRandomPos, generateRandomString } from "../tools/random.mjs";
import { arrayAvg } from "../tools/structures.mjs";
import { createEDNetwork } from "../instance-generator/index.mjs";
import LoRaWANModel from "../network-model/index.mjs";

const defaultParameters = {
    N: 10000,
    H: 3600,
    mapWidth: 1000, 
    mapHeight: 1000,
    posDistr: "normal",
    periodsDistr: "97, 1, 0, 2", // 97% -> 3600, 1% -> 1800, 0% -> 1200, 2% -> 900
    initialGW: 5,
    strategy: "random",
    maxIter: 1000,
    maxRuntime: 60,
    updateRate: 10
};

export default class Manager {
    constructor(params = defaultParameters) {
        this.configure(params);
        this.status = "Awaiting initialization";
        this._model = null;
        this.onChange = () => {};
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
            updateRate: this.updateRate
        };
    }

    getNetwork(attrs) {
        return this._model.getAllNodes(attrs);
    }

    initialize() {
        const instance = createEDNetwork(
            this.N, 
            this.H, 
            this.posDistr, 
            [this.mapWidth, this.mapHeight], 
            this.periodsDistr.split(",").map(p => parseInt(p))
        ); 

        this._model = new LoRaWANModel();
        instance.forEach(ed => {
            this._model.addEndDevice(ed.position, ed.period);
        });

        for(let i = 0; i < this.initialGW; i++) 
        this._model.addGateway(generateRandomPos([this.mapWidth, this.mapHeight]));        

        const start = Date.now();
        this._model.autoConnect();
        const elapsed = Date.now() - start;
        console.log(`Elapsed time: ${elapsed}ms`);

        this._simulationStep = 0;
        this.status = "Ready";
        this.onChange();
    }

    getNetworkStats() {
        const eds = this._model.getEndDevices();
        const totalCount = eds.length;
        const notConnectedCount = eds.filter(ed => ed.group === "NCED").length;
        const stats = {
            ufAvg: arrayAvg(this._model.getGateways().map(gw => gw.UF)),
            notConnectedCount: notConnectedCount,
            coverage: (1 - notConnectedCount/totalCount)*100
        };
        return stats;
    }

    importModel(data, format) {
        console.log("Importing...", format);
        console.log(data);
    }

    exportModel(format) {
        console.log("Exporting...", format);
        switch(format) {
            case "json":
            {
                const data = {}; // TODO
                return JSON.stringify(data);
            }
            case "csv":
            {
                const data = [[]]; // TODO
                return data.map(row => row.join(',')).join('\n');
            }
            case "matlab":
                return "Not implemented yet. Use csv for now.";
            default:
                return null;
        }
    }

    _run() {
        setTimeout(() => {
            this._model.disconnectEndDevices();
            this._model.moveGatewayIdx(0, generateRandomPos([this.mapWidth, this.mapHeight]));
            this._model.autoConnect();
            
            this._simulationStep++;
            
            if(this._simulationStep % this.updateRate === 0) 
                this.onChange();
            if((Date.now() - this._startTime)/1000 > this.maxRuntime)
                this.status = "Runtime exceeded";
            if(this._simulationStep > this.maxIter)
                this.status = "Iterations completed";
            if(this.status === "running") 
                this._run();
            else{
                const results = {
                    ...this.getNetworkStats(),
                    condition: this.status,
                    elapsed: Date.now() - this._startTime
                };
                this._returnResults(results);
            }
        }, 1);
    }

    start() {
        return new Promise(resolve => {
            this.status = "running";
            this._startTime = Date.now();
            this._returnResults = resolve;
            this._run();
        });
    }
}