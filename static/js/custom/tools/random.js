const generateRandomString = () => {
    var r = Math.random().toString(36).slice(2)+String(Date.now()).slice(-4);
    return r.padStart(16, '0');
};

// Uniform distribution U(-0.5, 0.5)
const uniformDist = () => Math.random() - 0.5;

// Normal distribution N(u, s^2)
const normalDist = (mu = 0, sigma = 1) => {
    // Box-Muller transform
    var u = Math.random();
    var v = Math.random();
    return sigma * Math.sqrt(-2.0 * Math.log(u)) * Math.cos(2.0 * Math.PI * v) + mu;
};

// Custom random distribution
const randomSelect = data => data[Math.floor(Math.random() * data.length)];

// Random position in x,y plane
const generateRandomPos = (ranges = [100, 100], dist = uniformDist) => {
    return {
        x: dist()*ranges[0],
        y: dist()*ranges[1]
    };
};

const generateRandomDistribution = (data, prob = [100]) => {
    // This functions returns an array of numbers sampled from "data" with frequencies
    // defined by the values passed in "prob". The values of prob[i] represents the 
    // probability of selecting data[i] if the result of this function is uniformly sampled.
    // The condition for prob is that sum(prob) = 100.
    let dataDist = [];
    if(arraySum(prob) === 100){
        for(let i = 0; i < prob.length; i++)
            dataDist = dataDist.concat(new Array(prob[i]).fill(data[i]));
        return dataDist;
    } else 
        throw new Error("Wrong distribution");
};
