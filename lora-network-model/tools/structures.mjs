export const arraySum = arr => arr.reduce((a, b) => a + b, 0);

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
