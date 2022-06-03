import { generateRandomPos, generateRandomString } from "../tools/random.mjs";
import { arrayAvg } from "../tools/structures.mjs";
import { createEDNetwork } from "../instance-generator/index.mjs";
import LoRaWANModel from "../network-model/index.mjs";

const defaultParameters = {
    N: 10000,
    H: 3600,
    mapWidth: 1000, 
    mapHeight: 1000,
    posDistr: "uniform",
    periodsDistr: "97, 1, 0, 2", // 97% -> 3600, 1% -> 1800, 0% -> 1200, 2% -> 900
    initialGW: 5,
    strategy: "random",
    maxIter: 100,
    maxRuntime: 60,
    updateRate: 10
};

export default class Manager {
    constructor(params = defaultParameters) {
        this.configure(params);
        this.status = "not-initialized";
        this._model = null;
        this.onChange = console.log;
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

    getResults = () => ({            
        ...this._model.getNetworkStats(),
        status: this.status,
        exitCondition: this.exitCondition,
        elapsed: this.status === "running" ? Date.now() - this._startTime : 0
    });

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

        this.startTime = Date.now();
        this._model.autoConnect();

        this._simulationStep = 0;
        this.status = "ready";
        this.onChange(this.getResults());
    }

    importModel(data, format) {
        console.log("Importing...", format);
        console.log(data);
        let params = null;
        switch(format) {
            case "json": {
                params = JSON.parse(data);        
                break;
            }
            case "csv": {
                if(data.lenth === 11 ){
                    params = {
                        N: data[0],
                        H: data[1],
                        mapWidth: data[2],
                        mapHeight: data[3],
                        posDistr: data[4],
                        periodsDistr: data[5],
                        initialGW: data[6],
                        strategy: data[7],
                        maxIter: data[8],
                        maxRuntime: data[9],
                        updateRate: data[10]
                    };
                }
                break;
            }
            default: {
                throw new Error("Unsupported format");
            }
        }
        if(params) {
            Object.assign(this, params);
            this.initialize();
        }else {
            throw new Error("Invalid data");
        }
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
                return new Error("Not implemented yet. Use csv for now.");
            default:
                return new Error("Unsupported format");
        }
    }

    _run() { // Single simulation step (async for updating GUI)
        setTimeout(() => {
            this._model.refactorGW([this.mapWidth, this.mapHeight], this.strategy);
            
            this._simulationStep++;
            
            if(this._simulationStep % this.updateRate === 0) 
                this.onChange(this.getResults());
            if((Date.now() - this._startTime)/1000 > this.maxRuntime){
                this.status = "ready";
                this.exitCondition = "timeout";
            }
            if(this._simulationStep > this.maxIter){
                this.status = "ready";
                this.exitCondition = "iterations completed";
            }
            if(this.status === "running") 
                this._run();
            else{
                this.onChange(this.getResults());
                this._returnResults(this.getResults());
            }
        }, 1);
    }

    start() { // Launch the simulation
        return new Promise(resolve => {
            this.status = "running";
            this.exitCondition = "";
            this._startTime = Date.now();
            this._returnResults = resolve;
            this._run();
        });
    }
}