var frames = [];
var gmapSize = 1000;

var onResultsUpdate = (iters, elapsed, exitCode, gws, coverage, channels) => {
    const exitCodes = [
        "Optimization not started",
        "Timeout",
        "Iterations completed",
        "100% Coverage reached"
    ];    
    Module.onResults({
        iters,
        elapsed,
        exitCode: exitCodes[exitCode],
        gws,
        coverage,
        channels
    });
};

const edgeColors = {
    7: "rgb(0, 0, 150, .7)",
    8: "rgb(150, 0, 0, .6)",
    9: "rgb(0, 150, 0, .5)",
    10: "rgb(150, 150, 0, .5)",
    11: "rgb(0, 150, 150, .4)",
    12: "rgb(150, 0, 150, .3)"
};
const nodeColors = {
    0: "rgb(250,0,0)",
    1: "rgb(0,0,250)",
    2: "rgb(0,250,0)"
};

var onNetworkUpdate = (x, y, id, group, from, to, sf, nodeslen, edgeslen) => {
    var xBuffer = Module.HEAPF64.subarray(x / 8, (x + nodeslen * 8) / 8);
    var yBuffer = Module.HEAPF64.subarray(y / 8, (y + nodeslen * 8) / 8);
    var idBuffer = Module.HEAPU32.subarray(id / 4, (id + nodeslen * 4) / 4);
    var groupBuffer = Module.HEAPU8.subarray(group, (group + nodeslen));

    var fromBuffer = Module.HEAPU32.subarray(from / 4, (from + edgeslen * 4) / 4);
    var toBuffer = Module.HEAPU32.subarray(to / 4, (to + edgeslen * 4) / 4);
    var sfBuffer = Module.HEAPU8.subarray(sf, (sf + edgeslen));

    var xArray = new Float64Array(xBuffer);
    var yArray = new Float64Array(yBuffer);
    var idArray = new Uint32Array(idBuffer);
    var groupArray = new Uint8Array(groupBuffer);
    var fromArray = new Uint32Array(fromBuffer);
    var toArray = new Uint32Array(toBuffer);
    var sfArray = new Uint8Array(sfBuffer);

    var nodes = [];
    var edges = [];

    for(var i = 0; i < nodeslen; i++) {                    
        nodes[idArray[i]] = {
            x: (xArray[i]/gmapSize+.5),
            y: (yArray[i]/gmapSize+.5),
            group: groupArray[i],
            color: nodeColors[groupArray[i]]
        };
    }

    for(var i = 0; i < edgeslen; i++) {
        edges.push({
            from: fromArray[i],
            to: toArray[i],
            sf: sfArray[i],
            color: edgeColors[sfArray[i]]
        });
    }

    frames.push({nodes,edges});

    Module._free(x);
    Module._free(y);
    Module._free(id);
    Module._free(group);
    Module._free(from);
    Module._free(to);
    Module._free(sf);

    Module.onUpdate();
};

var optimizerReady = false;
var Module = {
    onRuntimeInitialized: function() {
        optimizerReady = true;
    },
    onUpdate: function() {},
    onResults: function(res) {
        console.log("Optimization finished.");
        console.log(res);
    }
};

var runOptimizer = function(...params) {
    gmapSize = params[0];
    frames = [];
    const optimizer = new Module.JsInterface(...params);
    optimizer.run();
};