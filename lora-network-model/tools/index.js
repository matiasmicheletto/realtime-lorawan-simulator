export function generateRandomString() {
    var r = Math.random().toString(36).slice(2)+String(Date.now()).slice(-4);
    return r.padStart(16, '0');
};

export function generateRandomPos() {
    
    function uniform() { 
        return 2*Math.random() - 1;
    }

    return {
        x: uniform()*500,
        y: uniform()*500
    };
};