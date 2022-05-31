const createEDNetwork = (edNumber, hyperperiod, posDistr = uniformDist, mapSize = [100, 100], periodsDistr = [90, 10]) => {
    
    const divisors = getAllDivisors(hyperperiod);
    const periodsDist = generateRandomDistribution(divisors, periodsDistr);

    const nodes = [];
    for(let i = 0; i < edNumber; i++) {
        nodes.push({
            position: generateRandomPos(mapSize, posDistr),
            period: randomSelect(periodsDist)
        });
    }

    return nodes;
};