// Constants
const nodeColors = {
    0: "rgb(250,0,0)",
    1: "rgb(0,0,250)",
    2: "rgb(0,250,0)"
};
const edgeColors = {
    7: "rgb(0, 0, 150, .7)",
    8: "rgb(150, 0, 0, .6)",
    9: "rgb(0, 150, 0, .5)",
    10: "rgb(150, 150, 0, .5)",
    11: "rgb(0, 150, 150, .4)",
    12: "rgb(150, 0, 150, .3)"
};

const frames = [];
let currentFrame = 0;
const canvas = document.getElementById("canvas");
const ctx = canvas.getContext("2d");


// Default parameters for optimization
const mapSize = 2000;
const edNumber= 40000;
const maxSF = 10;
const posDist = 0; // 0:uniform, 1:normal, 2:clouds
const periodDist = 0; // 0:soft, 1:medium, 2:hard
const maxIter = 1000;
const timeout = 300;
const algorithm = 0; // 0:springs, 1:random
const updateRate = 50; // Update results every "updateRate" iterations

const scale = mapSize/1000;
canvas.width = mapSize/scale;
canvas.height = mapSize/scale;

// Callback for optimization steps
const onNetworkUpdate = (x, y, id, group, from, to, sf, nodeslen, edgeslen) => {
    
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
        //console.log(i, idArray[i]);
        nodes[idArray[i]] = {
            x: (xArray[i]+mapSize/2)/scale,
            y: (yArray[i]+mapSize/2)/scale,
            group: groupArray[i]
        };
    }
    for(var i = 0; i < edgeslen; i++) {
        edges.push({
            from: fromArray[i],
            to: toArray[i],
            color: edgeColors[sfArray[i]]
        });
    }

    frames.push({nodes,edges});
    if(frames.length === 1)
        drawNetwork();

    Module._free(x);
    Module._free(y);
    Module._free(id);
    Module._free(group);
    Module._free(from);
    Module._free(to);
    Module._free(sf);
}

// Callback for optimization finish
const onResultsUpdate = (iters, elapsed, exitCode, gws, coverage, channels) => {

    const exitCodes = [
        "Optimization not started",
        "Timeout",
        "Iterations completed",
        "100% Coverage reached"
    ];
    
    console.log("Optimization finished");
    console.log("Coverage:", coverage*100);
    console.log("Number of gateways:", gws);
    console.log("Channels:", channels);
    console.log("Iterations:",iters);
    console.log("Exit condition:", exitCodes[exitCode]);
    console.log("Elapsed:", elapsed, "ms");
}

// Update chart
const drawNetwork = () => {
    setTimeout(() => {
        const iter = frames[currentFrame];
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        for(var node of iter.nodes) {
            ctx.fillStyle = nodeColors[node.group];
            ctx.fillRect(node.x, node.y, 1, 1);
        }
        for(var edge of iter.edges) {
            const fromNode = iter.nodes[edge.from];
            const toNode = iter.nodes[edge.to];
            ctx.beginPath();
            ctx.moveTo(fromNode.x, fromNode.y);
            ctx.lineTo(toNode.x, toNode.y);
            ctx.strokeStyle = edge.color;
            ctx.stroke();
        }
        currentFrame++;
        if(currentFrame < frames.length)
            drawNetwork();
    },100);
}

var Module = {
    onRuntimeInitialized: function() {
        const optimizer = new Module.JsInterface(
            mapSize,
            edNumber,
            maxSF,
            posDist,
            periodDist,
            maxIter,
            timeout,
            algorithm,
            updateRate
        );
        optimizer.run();
    }
}

const repeatBtn = document.getElementById("repeatBtn");
repeatBtn.onclick = () => {
    currentFrame = 0;
    drawNetwork();
}