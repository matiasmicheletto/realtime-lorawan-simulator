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


const canvas = document.getElementById("canvas");
const runBtn = document.getElementById("runBtn");
const repeatBtn = document.getElementById("repeatBtn");

var frames = [];
var currentFrame = 0;
const ctx = canvas.getContext("2d");
canvas.width = window.innerWidth;
canvas.height = canvas.width;
var scale = 1;

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
            x: (xArray[i]+mapSize/2),
            y: (yArray[i]+mapSize/2),
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
    if(frames.length == 1)
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
    runBtn.disabled = false;
}

// Update chart
const drawNetwork = () => {
    const iter = frames[currentFrame];
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    for(var node of iter.nodes) {
        ctx.fillStyle = nodeColors[node.group];
        ctx.fillRect(node.x*scale, node.y*scale, 1, 1);
    }

    for(var edge of iter.edges) {
        const fromNode = iter.nodes[edge.from];
        const toNode = iter.nodes[edge.to];
        ctx.beginPath();
        ctx.moveTo(fromNode.x*scale, fromNode.y*scale);
        ctx.lineTo(toNode.x*scale, toNode.y*scale);
        ctx.strokeStyle = edge.color;
        ctx.stroke();
    }
    
    if(currentFrame < frames.length-1){
        currentFrame++;
        setTimeout(drawNetwork, 100);
    }else 
        repeatBtn.disabled = false;
}

var optimizerReady = false;
var Module = {
    onRuntimeInitialized: function() {
        optimizerReady = true;
    }
}

runBtn.onclick = () => {
    runBtn.disabled = true;
    mapSize = parseInt(document.getElementById("mapSize").value);
    const edNumber= parseInt(document.getElementById("edNumber").value);
    const maxSF = parseInt(document.getElementById("maxSF").value);
    const posDist = parseInt(document.getElementById("posDist").value);
    const periodDist = parseInt(document.getElementById("periodDist").value);
    const maxIter = parseInt(document.getElementById("maxIter").value);
    const timeout = parseInt(document.getElementById("timeout").value);
    const algorithm = parseInt(document.getElementById("algorithm").value);
    const updateRate = parseInt(document.getElementById("updateRate").value);
    
    scale = canvas.width/mapSize;
    frames = [];
    currentFrame = 0;

    if(optimizerReady) {
        runBtn.disabled = true;
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

repeatBtn.onclick = () => {
    repeatBtn.disabled = true;
    currentFrame = 0;
    drawNetwork();
}

window.onresize = () => {
    canvas.width = window.innerWidth;
    canvas.height = canvas.width;
    scale = canvas.width/mapSize;
    drawNetwork();
}