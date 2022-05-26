import { arraySum } from "./structures.mjs";

export const generateRandomString = () => {
    var r = Math.random().toString(36).slice(2)+String(Date.now()).slice(-4);
    return r.padStart(16, '0');
};

// Uniform distribution U(-1, 1)
export const uniformDist = () => 2*Math.random() - 1;

// Gauss distribution N(0, 1)
export const gaussDist = () => {
    // Box-Muller transform
    var u = Math.random();
    var v = Math.random();
    return Math.sqrt(-2.0 * Math.log(u)) * Math.cos(2.0 * Math.PI * v);
}

// Custom random distribution
export const randomSelect = data => data[Math.floor(Math.random() * data.length)];

// Random position in x,y plane
export const generateRandomPos = (dist = uniformDist) => {
    return {
        x: dist()*500,
        y: dist()*500
    };
};

export const generateRandomDistribution = (data, dist = [100]) => {
    // This functions returns an array of numbers sampled from "data" with
    // the distribution passed in "dist"
    let dataDist = [];
    if(arraySum(dist) === 100){
        for(let i = 0; i < dist.length; i++)
            dataDist = dataDist.concat(new Array(dist[i]).fill(data[i]));
        return dataDist;
    } else 
        throw new Error("Wrong distribution");
}
