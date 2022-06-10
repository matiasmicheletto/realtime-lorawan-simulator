import { generateRandomPos } from "../tools/random.mjs";
import LoRaWANModel from "../network-model/index.mjs";

const defaultParameters = {
    N: 6500, // Number of EDs
    H: 3600, // Hyperperiod of the system
    mapWidth: 1000, 
    mapHeight: 1000,
    posDistr: "uniform", // Distribution of EDs in map
    periodsDistr: "97, 1, 0, 2", // 97% -> 3600, 1% -> 1800, 0% -> 1200, 2% -> 900
    initialGW: 1, // Number of GW to begin with
    adaptiveGWNumber: false, // Remove GW if coverage is 100%
    minCoverage: 100, // Minimum coverage to add GW
    strategy: "spring", // Strategy for moving gateways positions
    schedulingBy: "ed", // Schedule by GW or by ED
    maxIter: 100, // Max iterations to run
    maxRuntime: 60, // Seconds 
    addGWAfter: 25, // After how many iterations with cov < 80% should a new GW be added
    updateRate: 10, // How often should the model status be updated
};

export default class Manager {
    constructor(params) {
        this.configure({
            ...defaultParameters,
            ...params
        });
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
            adaptiveGWNumber: this.adaptiveGWNumber,
            minCoverage: this.minCoverage,
            strategy: this.strategy,
            schedulingBy: this.schedulingBy,
            maxIter: this.maxIter,
            maxRuntime: this.maxRuntime,
            addGWAfter: this.addGWAfter,
            updateRate: this.updateRate
        };
    }

    getNetwork(attrs) {
        return this._model.getAllNodes(attrs);
    }

    getResults = () => ({            
        ...this._model.getNetworkStats(),
        status: this.status,
        iteration: this._simulationStep,
        suboptimalSteps: this._suboptimalSteps,
        exitCondition: this.exitCondition,
        elapsed: Date.now() - this._startTime
    });

    initialize() {
        this._model = new LoRaWANModel(
            this.N, 
            this.H, 
            this.posDistr, 
            [this.mapWidth, this.mapHeight], 
            this.periodsDistr.split(",").map(p => parseInt(p))
        );

        for(let i = 0; i < this.initialGW; i++) 
            this._model.addGateway(generateRandomPos([this.mapWidth, this.mapHeight]));        

        this._model.setRefactorStepMethod(this.strategy);
        this._model.setAutoConnectMethod(this.schedulingBy);

        this._startTime = Date.now();
        this._model.autoConnect();

        this._simulationStep = 0;
        this._suboptimalSteps = 0;
        this.status = "ready";
        this.onChange(this.getResults());
    }

    importConfig(data, format) {
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
                        adaptiveGWNumber: data[7],
                        strategy: data[8],
                        schedulingBy: data[9],
                        maxIter: data[10],
                        addGWAfter: data[11],
                        maxRuntime: data[12],
                        updateRate: data[13]
                    }
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
                const data = {
                    config: this.getAllParams(),
                    nodes: this._model.getAllNodes()
                };
                return JSON.stringify(data);
            }
            case "csv":
            {
                const nodes = this._model.getAllNodes();
                const header = [
                    "ID",
                    "Label",
                    "Group",
                    "Lat",
                    "Long",
                    "Period",
                    "SF",
                    "UF",
                    "Channel",
                    "Connected to"
                ];
                const data = nodes.map(n => [
                    n.id,
                    n.label,
                    n.group,
                    n.x,
                    n.y,
                    n.period,
                    n.sf,
                    n.UF,
                    n.channel,
                    n.connectedTo
                ]);
                return [header, ...data].map(row => row.join(',')).join('\n');
            }
            case "matlab":
                return new Error("Not implemented yet. Use csv for now.");
            default:
                return new Error("Unsupported format");
        }
    }

    _run() { // Single simulation step (async for updating GUI)
        setTimeout(() => {
            this._model.refactorStep();
            const res = this.getResults();

            // Adaptive GW number model
            if(this.adaptiveGWNumber && res.coverage === 100 && this._model._gateways.length > 1) {
                console.log("Max coverage reached, removing GW");
                this._suboptimalSteps = 0;
                this._model.disconnectGateway(0, true);
                this._model.autoConnect();
            }else{
                if(res.coverage < this.minCoverage) {
                    this._suboptimalSteps++;
                    if(this._suboptimalSteps === this.addGWAfter){
                        this._suboptimalSteps = 0;
                        this._model.addGateway(generateRandomPos([this.mapWidth, this.mapHeight]));
                        this._model.autoConnect();
                    }
                }
            }

            // Exit conditions
            this._simulationStep++;
            if(this._simulationStep > this.maxIter){
                this.status = "ready";
                this.exitCondition = "iterations completed";
            }else{
                if(this._simulationStep % this.updateRate === 0)
                    this.onChange(res);
            }
            
            if(res.coverage === 100 && this.strategy === "spring") {
                this.status = "ready";
                this.exitCondition = "Max. coverage reached";
            }

            if((Date.now() - this._startTime)/1000 > this.maxRuntime){
                this.status = "ready";
                this.exitCondition = "timeout";
            }
            
            if(this.status === "running") 
                this._run();
            else{
                this.onChange(res);
                this._returnResults(this.getResults()); // To call for resolve
            }
        }, 1);
    }

    start() { // Launch the simulation
        return new Promise(resolve => {
            this.onChange(this.getResults());
            this.status = "running";
            this.exitCondition = "";
            this._startTime = Date.now();
            this._returnResults = resolve;
            this._simulationStep = 0;
            this._suboptimalSteps = 0;
            this._run();
        });
    }
}