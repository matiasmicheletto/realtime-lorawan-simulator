var nodes, edges, network;

function draw() {
    nodes = new vis.DataSet([{
            id: "1",
            label: "Gateway 1",
            group: "GW",
            x: 0,
            y: 0
        },
        {
            id: "2",
            label: "End device 1",
            group: "ED",
            x: -100,
            y: -100
        },
        {
            id: "3",
            label: "End device 2",
            group: "ED",
            x: 500,
            y: -200
        },
        {
            id: "4",
            label: "End device 3",
            group: "ED",
            x: 500,
            y: 500
        },
        {
            id: "5",
            label: "End device 4",
            group: "ED",
            x: -500,
            y: 0
        },
    ]);

    edges = new vis.DataSet([{
            id: "1",
            from: "2",
            to: "1",
            arrows: {
                to: {
                    enabled: true,
                    type: "arrow",
                },
            }
        },
        {
            id: "2",
            from: "3",
            to: "1",
            arrows: {
                to: {
                    enabled: true,
                    type: "arrow",
                },
            }
        },
        {
            id: "3",
            from: "4",
            to: "1",
            arrows: {
                to: {
                    enabled: true,
                    type: "arrow",
                },
            }
        },
        {
            id: "4",
            from: "5",
            to: "1",
            arrows: {
                to: {
                    enabled: true,
                    type: "arrow",
                },
            }
        },
    ]);

    var container = document.getElementById("network");
    var data = {
        nodes: nodes,
        edges: edges,
    };
    var options = {
        edges: {
            smooth: false
        },
        physics: false,
        interaction: {
            dragNodes: false
        },
        groups: {
            ED: {
                shape: "dot",
                color: "rgb(0,0,250)"
            },
            GW: {
                shape: "triangle",
                color: "rgb(250,0,0)"
            }
        }
    };
    network = new vis.Network(container, data, options);
}