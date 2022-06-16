const visContainer = document.getElementById("network");
var onDataUpdate = (nodes, edges) => {

    const data = {
        nodes: new vis.DataSet(nodes),
        edges: new vis.DataSet(edges),
    };
            
    const options = {
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
    
    const net = new vis.Network(visContainer, data, options);
}