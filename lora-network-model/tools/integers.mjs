// Greatest common divisor
export const gcd = (a, b) => {
    let aa = Math.abs(a);
    let ab = Math.abs(b);
    while(ab) {
        let t = ab;
        ab = aa % ab;
        aa = t;
    }
    return aa;
}

// Least common multiple
const lcm2 = (a, b) => (!a || !b) ? 0 : Math.abs(a * b) / gcd(a, b);
export const lcm = arr => arr.reduce(lcm2); // Extended for arrays

// Get list of integer divisors of a number
export const getAllDivisors = n => {
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
