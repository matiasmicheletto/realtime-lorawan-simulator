const data = {
    nodes: new vis.DataSet(model.getAllNodes(visNodeAttrs)),
    edges: new vis.DataSet(model.getLinks(visLinkAttrs)),
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
            size: 5,
            color: "rgb(250,0,0)"
        },
        ED: { // Connected end devices
            shape: "dot",
            size: 5,
            color: "rgb(0,0,250)"
        },
        GW: { // Gateways
            shape: "triangle",
            size: 10,
            color: "rgb(0,250,0)"
        }
    }
};
const el = document.getElementById("network");
const net = new vis.Network(el, data, options);