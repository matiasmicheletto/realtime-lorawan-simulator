import Manager from '../model-manager/index.mjs';

const config = {
    N: 6500,
    H: 3600,
    mapWidth: 1000, 
    mapHeight: 1000,
    posDistr: "uniform",
    periodsDistr: "97, 1, 0, 2", // 97% -> 3600, 1% -> 1800, 0% -> 1200, 2% -> 900
    initialGW: 3,
    strategy: "random",
    schedulingBy: "ed",
    maxIter: 100,
    maxRuntime: 60,
    updateRate: 0.1
};

const manager = new Manager(config);
manager.initialize();
manager.start();