var optimizerReady = false;
var Module = {
    onRuntimeInitialized: function() {
        optimizerReady = true;
    },
    onNetworkUpdate: (nodes, edges, gw, ced, nced, iter, maxSF) => {
        console.log(nodes, edges, gw, ced, nced, iter, maxSF);
    },
    onResultsUpdate: (iters,elapsed,exitCode,gws,coverage,channels) => {
        console.log(iters,elapsed,exitCode,gws,coverage,channels);
    },
    edgeColors: {
        7: "rgb(0, 0, 150, .7)",
        8: "rgb(150, 0, 0, .6)",
        9: "rgb(0, 150, 0, .5)",
        10: "rgb(150, 150, 0, .5)",
        11: "rgb(0, 150, 150, .4)",
        12: "rgb(150, 0, 150, .3)"
    },
    nodeColors: {
        0: "rgb(250,0,0)",
        1: "rgb(0,0,250)",
        2: "rgb(0,250,0)"
    }
};