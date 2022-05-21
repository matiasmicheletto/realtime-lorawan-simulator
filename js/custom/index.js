function update() {

    var nodes = [
        {
            id: "gw1",
            label: "Gateway 1",
            group: "GW",
            x: -150,
            y: -10
        },
        {
            id: "gw2",
            label: "Gateway 2",
            group: "GW",
            x: 120,
            y: 10
        }
    ];
    var edges = [];

    var N = document.getElementById("input-number").value;

    for(var idx = 0; idx < N; idx++){
        var nodeId = generateRandomString();
        nodes.push({
            id: nodeId,
            label: `End device ${idx+1}`,
            group: "ED",
            ...generateRandomPos()
        });
        edges.push({
            id: generateRandomString(),
            from: nodeId,
            to: Math.random() > 0.5 ? "gw1":"gw2",
            /*
            arrows: {
                to: {
                    enabled: true,
                    type: "arrow",
                },
            }
            */
        });
    }

    var data = {
        nodes: new vis.DataSet(nodes),
        edges: new vis.DataSet(edges),
    };
    var options = {
        height: "80%",        
        edges: { smooth: false },
        physics: false,
        interaction: { dragNodes: false },
        groups: {
            ED: {
                shape: "dot",
                size: 5,
                color: "rgb(0,0,250)"
            },
            GW: {
                shape: "triangle",
                size: 10,
                color: "rgb(250,0,0)"
            }
        }
    };
    var container = document.getElementById("network");
    return new vis.Network(container, data, options);
}


var fileInput = document.getElementById("input-file");    
fileInput.onchange = function(e) {
    readFile(e.target.files[0], "csv")
    .then(result => console.log(result));
};
var numberInput = document.getElementById("input-number");
numberInput.onchange = function(e) {
    update();
};
window.onload = update;