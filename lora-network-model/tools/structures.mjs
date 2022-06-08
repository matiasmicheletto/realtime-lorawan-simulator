export const arraySum = arr => arr.reduce((a, b) => a + b, 0);

export const arrayAvg = arr => arraySum(arr) / arr.length;

// Get a subset of attributes of an object
export const selectAttributes = (data, attrs) => data.map(el =>
    attrs.filter(key => key in el).reduce((acc, key) => (acc[key] = el[key], acc), {})
);

export const memoize = fn => {
    let cache = {};
    return (...args) => {
        let n = args[0]; 
        if (n in cache)
            return cache[n];
        else {
            let result = fn(n);
            cache[n] = result;
            return result;
        }
    }
};

export const objToText = obj => {
    let text = "";
    for (let key in obj) {
        text += `${key},${obj[key]}\n`;
    }
    return text;
}

export const objArrToCsv = arr => {
    const header = Object.keys(arr[0]);
    const data = arr.map(el => Object.values(el));
    return [header, ...data].map(row => row.join(',')).join('\n');
}
