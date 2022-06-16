var getData = (x, y, id, group, from, to, sf, nodeslen, edgeslen) => {
    
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

    var groups = ["NCED", "ED", "GW"];
    const edgeColors = {
        7: "rgb(0, 0, 150)",
        8: "rgb(150, 0, 0)",
        9: "rgb(0, 150, 0)",
        10: "rgb(150, 150, 0)",
        11: "rgb(0, 150, 150)",
        12: "rgb(150, 0, 150)"
    };

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

    onDataUpdate(nodes, edges);

    Module._free(x);
    Module._free(y);
    Module._free(id);
    Module._free(group);
    Module._free(from);
    Module._free(to);
    Module._free(sf);
}

var Module = {
    onRuntimeInitialized: function() {

        const mapSize = 1000;
        const edNumber= 6000;
        const maxIter = 100;
        const timeout = 60;
        const algorithm = 0;
        const updateRate = 10;

        const optimizer = new Module.OptimizerInterface(
            mapSize,
            edNumber,
            maxIter,
            timeout,
            algorithm,
            updateRate
        );
        optimizer.run();
    }
}