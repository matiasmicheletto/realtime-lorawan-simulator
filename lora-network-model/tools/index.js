export const generateRandomString = () => {
    var r = Math.random().toString(36).slice(2)+String(Date.now()).slice(-4);
    return r.padStart(16, '0');
};

const uniform = () => { 
    // Uniform distribution
    return 2*Math.random() - 1;
}

const gauss = () => {
    // Box-Muller transform
    var u = Math.random();
    var v = Math.random();
    return Math.sqrt(-2.0 * Math.log(u)) * Math.cos(2.0 * Math.PI * v);
}

export const generateRandomPos = (dist = uniform) => {
    return {
        x: dist()*500,
        y: dist()*500
    };
};

const getDivisors = n => {    
    const divs = [];
    for(let i = 1; i < Math.sqrt(n) + 1; i++) {
        if(n % i == 0) {
            divs.push(i);
            if(i*i != n) 
            divs.push(n / i);
        }
    }
    divs.sort((a, b) => b-a);
    return divs;
}

// Memoized divisors
let divisors = {    
    '2': [1, 2],    
    '4': [1, 2, 4]
};

export const getRandomDivisor = (H, condition = n => n > 100) => {
    if(!divisors[H])
        divisors[H] = getDivisors(H);
    const tries = 10;
    let idx = Math.floor(Math.random() * divisors[H].length);
    let selected = divisors[H][idx];
    while(tries > 0 && !condition(selected)) {
        idx = Math.floor(Math.random() * divisors[H].length);
        selected = divisors[H][idx];
    }
    return selected;   
}

export const selectAttributes = (data, attrs) => data.map(el =>
    attrs.filter(key => key in el).reduce((acc, key) => (acc[key] = el[key], acc), {})
);