import React, { useContext, useEffect, useRef, useState } from 'react';
import { Checkbox, FormControlLabel, Grid, Box } from '@mui/material';
import { generateRandomString } from 'lora-network-model/tools/random';
import { Network } from 'vis-network';
import { DataSet } from 'vis-data';
import { manager, ModelContext } from '../../context';

const styles = {
    container: {
        width:"100%", 
        height:"75vh",
        border: "1px solid #bbb",
        boxShadow: "3px 3px 10px gray",
        borderRadius: "1%"
    },
    form: {
        padding: "20px"
    }
}

const nodeGroups = {
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
};

const edgeColors = {
    SF7: "rgb(0, 0, 150)",
    SF8: "rgb(150, 0, 0)",
    SF9: "rgb(0, 150, 0)",
    SF10: "rgb(150, 150, 0)",
    SF11: "rgb(0, 150, 150)",
    SF12: "rgb(150, 0, 150)"
};

const NetworkViewer = () => {

    const { model } = useContext(ModelContext);

    const netRef = useRef(null);
    const [filters, setFilters] = useState({
        ED: true,
        NCED: true,
        GW: true,
        labels: false
    });
    const [edgesFilter, setEdgesFilter] = useState(Object.keys(edgeColors).map(el => true)); // SF7, SF8, SF9, SF10, SF11, SF12

    const toggleSingeEdge = (idx, checked) => {
        let newEdgesFilter = [...edgesFilter];
        newEdgesFilter[idx] = checked;
        setEdgesFilter(newEdgesFilter);
    };
    const toggleEdges = checked => setEdgesFilter(new Array(6).fill(checked));

    const attrs = filters.labels ? ["id","group","x","y","connectedTo","label","sf"] : ["id","group","x","y","connectedTo","sf"];
    const nodes = manager.getNetwork(attrs).filter(node => 
        filters.ED && node.group === "ED" || filters.NCED && node.group === "NCED" || filters.GW && node.group === "GW"
    );
    const edges = nodes.filter(node => 
                node.group === "ED" && (
                    edgesFilter[0] && node.sf === "SF7" ||
                    edgesFilter[1] && node.sf === "SF8" ||
                    edgesFilter[2] && node.sf === "SF9" ||
                    edgesFilter[3] && node.sf === "SF10" ||
                    edgesFilter[4] && node.sf === "SF11" ||
                    edgesFilter[5] && node.sf === "SF12"
                )
            ).map(node => ({ // A link for each connected ED
                id: generateRandomString(),
                from: node.id,
                to: node.connectedTo,
                color: edgeColors[node.sf]
            }));
    const networkData = {
        nodes: new DataSet(nodes),
        edges: new DataSet(edges)
    };
    const options = {
        height: "100%",   
        width: "100%",
        edges: { smooth: false },
        physics: false,
        interaction: { dragNodes: false },
        groups: nodeGroups
    };
    useEffect(()=>{
        netRef.current && new Network(netRef.current, networkData, options);
    }, [netRef, filters, edgesFilter, model]);

    return (
        <div style={{marginTop: "20px"}}>
            <Grid container spacing={2} alignItems="center">
                <Grid item xs={3}>
                    <div style={{...styles.container, ...styles.form}}>
                        <h4 style={{padding:0, margin:0}}>Visualization filters</h4>
                        <FormControlLabel
                            control={
                                <Checkbox
                                    checked={filters.ED}
                                    onChange={e => setFilters({...filters, ED: e.target.checked})}/>
                            }
                            label="Connected ED"
                        />
                        <br />
                        <FormControlLabel
                            control={
                                <Checkbox
                                    checked={filters.NCED}
                                    onChange={e => setFilters({...filters, NCED: e.target.checked})}/>
                            }
                            label="Disconnected ED"
                        />
                        <br />
                        <FormControlLabel
                            control={
                                <Checkbox
                                    checked={filters.GW}
                                    onChange={e => setFilters({...filters, GW: e.target.checked})}/>
                            }
                            label="Gateways"
                        />
                        <br />
                        <FormControlLabel
                            control={
                                <Checkbox
                                    checked={filters.labels}
                                    onChange={e => setFilters({...filters, labels: e.target.checked})}/>
                            }
                            label="Labels"
                        />
                        <br />
                        <div>
                            <FormControlLabel
                                control={
                                    <Checkbox
                                        checked={edgesFilter.every(edge => edge)}
                                        indeterminate={edgesFilter.some(edge => edge) && !edgesFilter.every(edge => edge)}
                                        onChange={ e => toggleEdges(e.target.checked) } />
                                }
                                label="Links"
                            />
                            <Box sx={{ display: 'flex', flexDirection: 'column', ml: 3 }}>
                                {
                                    Object.keys(edgeColors).map( (sf, index) => (
                                        <FormControlLabel
                                            key={index}
                                            label={sf}
                                            control={
                                                <Checkbox 
                                                    checked={edgesFilter[index]} 
                                                    onChange={e => toggleSingeEdge(index, e.target.checked)} />
                                            }
                                        />
                                    ))
                                }    
                            </Box>
                        </div>
                    </div>
                </Grid>
                <Grid item xs={9}>
                    <div ref={netRef} style={styles.container}></div>
                </Grid>
            </Grid>
        </div>
    );
};

export default NetworkViewer;