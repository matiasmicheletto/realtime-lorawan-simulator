import Manager from '../model-manager/index.mjs';
import { objArrToCsv } from '../tools/structures.mjs';
import fs from 'fs';

const initialConfig = {
    N: 5000,
    H: 3600,
    mapWidth: 1000, 
    mapHeight: 1000,
    posDistr: "uniform",
    periodsDistr: "97, 1, 0, 2", // 97% -> 3600, 1% -> 1800, 0% -> 1200, 2% -> 900
    initialGW: 2,
    strategy: "random",
    schedulingBy: "ed",
    maxIter: 500,
    addGWAfter: 25,
    maxRuntime: 300, // 5 minutes
    updateRate: 0.1
};
const manager = new Manager(initialConfig);

const tests = [
    {
        name: "500_1000",
        mapWidth: 500, 
        mapHeight: 500,
        N: 1000,
        maxIter: 100
    },
    {
        name: "1000_1000",
        mapWidth: 1000, 
        mapHeight: 1000,
        N: 1000,
        maxIter: 100
    },
    {
        name: "2000_1000",
        mapWidth: 2000, 
        mapHeight: 2000,
        N: 1000,
        maxIter: 100
    },
    {
        name: "4000_1000",
        mapWidth: 4000, 
        mapHeight: 4000,
        N: 1000,
        maxIter: 100
    },
    {
        name: "500_2500",
        mapWidth: 500, 
        mapHeight: 500,
        N: 2500,
        maxIter: 200
    },
    {
        name: "1000_2500",
        mapWidth: 1000, 
        mapHeight: 1000,
        N: 2500,
        maxIter: 200
    },
    {
        name: "2000_2500",
        mapWidth: 2000, 
        mapHeight: 2000,
        N: 2500,
        maxIter: 200
    },
    {
        name: "4000_2500",
        mapWidth: 4000, 
        mapHeight: 4000,
        N: 2500,
        maxIter: 200
    },
    {
        name: "500_5000",
        mapWidth: 500, 
        mapHeight: 500,
        N: 5000,
        maxIter: 300
    },
    {
        name: "1000_5000",
        mapWidth: 1000, 
        mapHeight: 1000,
        N: 5000,
        maxIter: 300
    },
    {
        name: "2000_5000",
        mapWidth: 2000, 
        mapHeight: 2000,
        N: 5000,
        maxIter: 300
    },
    {
        name: "4000_5000",
        mapWidth: 4000, 
        mapHeight: 4000,
        N: 5000,
        maxIter: 300
    },
    {
        name: "500_10000",
        mapWidth: 500, 
        mapHeight: 500,
        N: 10000,
        maxIter: 500
    },
    {
        name: "1000_10000",
        mapWidth: 1000, 
        mapHeight: 1000,
        N: 10000,
        maxIter: 500
    },
    {
        name: "2000_10000",
        mapWidth: 2000, 
        mapHeight: 2000,
        N: 10000,
        maxIter: 500
    },
    {
        name: "4000_10000",
        mapWidth: 4000, 
        mapHeight: 4000,
        N: 10000,
        maxIter: 500
    }
];


let config = {};
let currentTest = -1;
const next = () => {
    currentTest++;
    if (currentTest < tests.length) {
        config = tests[currentTest];
        const path = `./data/result_${config.name}.txt`;
        if(!fs.existsSync(path)){ // Not overwrite if file already exists
            manager.configure(config);
            manager.initialize();
            manager.start().then(results => {
                fs.writeFile(path, objArrToCsv([results]), 'utf8', err => {
                    if (err) throw err;
                    console.log('File saved!');
                    next();
                });
            });
        }else{
            console.log(`${path} already exists`);
            next();
        }
    } else {
        console.log("Done");
        return;
    }
};
next();