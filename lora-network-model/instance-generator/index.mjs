import { 
    generateRandomDistribution,
    generateRandomPos,
    randomSelect
} from '../tools/random.mjs';
import { getAllDivisors } from '../tools/integers.mjs';

export const createEDNetwork = (edNumber, hyperperiod) => {
    
    const divisors = getAllDivisors(hyperperiod);
    const periodsDist = generateRandomDistribution(divisors, [95, 3, 2]);

    const nodes = [];
    for(let i = 0; i < edNumber; i++) {
        nodes.push({
            position: generateRandomPos(), // TODO: select distribution
            period: randomSelect(periodsDist)
        });
    }

    return nodes;
};