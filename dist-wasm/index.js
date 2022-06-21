const visContainer = document.getElementById("network");

// Parameters for optimization
const mapSize = 1000;
const edNumber= 5000;
const posDist = 2; // 0:uniform, 1:normal, 2:clouds
const periodDist = 0; // 0:soft, 1:medium, 2:hard
const maxIter = 100;
const timeout = 60;
const algorithm = 2;
const updateRate = 10;

// Constants
const groups = ["NCED", "ED", "GW"];
const edgeColors = {
    7: "rgb(0, 0, 150)",
    8: "rgb(150, 0, 0)",
    9: "rgb(0, 150, 0)",
    10: "rgb(150, 150, 0)",
    11: "rgb(0, 150, 150)",
    12: "rgb(150, 0, 150)"
};
const exitCodes = [
    "Optimization not started",
    "Timeout",
    "Iterations completed",
    "100% Coverage reached",
    "Maximum gateways reached"
];
const visOptions = {
    height: "100%",   
    width: "100%",
    edges: { smooth: false },
    physics: false,
    interaction: { dragNodes: false },
    groups: {
        NCED: { // Not connected end devices
            shape: "dot",
            size: 1,
            color: "rgb(250,0,0)"
        },
        ED: { // Connected end devices
            shape: "dot",
            size: 1,
            color: "rgb(0,0,250)"
        },
        GW: { // Gateways
            shape: "triangle",
            size: 3,
            color: "rgb(0,250,0)"
        }
    }
};

const frames = [];

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
        nodes.push({
            id: idArray[i],
            x: xArray[i],
            y: yArray[i],
            group: groups[groupArray[i]]
        });
    }
    for(var i = 0; i < edgeslen; i++) {
        edges.push({
            from: fromArray[i],
            to: toArray[i],
            color: edgeColors[sfArray[i]]
        });
    }

    frames.push({nodes,edges});
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
const onResultsUpdate = (iters, elapsed, coverage, exitCode) => {
    console.log("Optimization finished");
    console.log("Iterations:",iters);
    console.log("Elapsed:", elapsed);
    console.log("Coverage:", coverage*100);
    console.log("Exit condition:", exitCodes[exitCode]);
}

// Update chart
const drawNetwork = () => {
    setTimeout(() => {
        const it = frames.shift();
        const data = {
            nodes: new vis.DataSet(it.nodes),
            edges: new vis.DataSet(it.edges),
        };
        //console.log(it.nodes);
        const net = new vis.Network(visContainer, data, visOptions);
    }, 500);
}

var Module = {
    onRuntimeInitialized: function() {
        const optimizer = new Module.JsInterface(
            mapSize,
            edNumber,
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
