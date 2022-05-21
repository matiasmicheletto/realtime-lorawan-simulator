function onload() {

    var input = document.getElementById("input-file");    
    input.onchange = function(e) {
        readFile(e.target.files[0], "csv")
        .then(result => console.log(result));
    };

    var nodes = [
        {
            id: "gw",
            label: "Gateway",
            group: "GW",
            x: 0,
            y: 0
        }
    ];
    var edges = [];

    var N = 100;

    for(var idx = 0; idx < N; idx++){
        var nodeId = generateRandomString();
        nodes.push({
            id: nodeId,
            //label: `End device ${idx+1}`,
            group: "ED",
            x: Math.random()*1000 - 500,
            y: Math.random()*1000 - 500
        });
        edges.push({
            id: generateRandomString(),
            from: nodeId,
            to: "gw",
            /*
            arrows: {
                to: {
                    enabled: true,
                    type: "arrow",
                },
            }*/
        });
    }

    var data = {
        nodes: new vis.DataSet(nodes),
        edges: new vis.DataSet(edges),
    };
    var options = {
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
    var network = new vis.Network(container, data, options);
}