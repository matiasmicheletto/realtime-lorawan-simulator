import React, { useContext, useEffect, useRef, useState } from 'react';
import { Checkbox, FormControlLabel, Grid } from '@mui/material';
import { generateRandomString } from 'lora-network-model/tools/random';
import { Network } from 'vis-network';
import { DataSet } from 'vis-data';
import { ModelCtx } from '../../context';

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

const LoRaNetwork = () => {

    const netRef = useRef(null);
    const model = useContext(ModelCtx);
    const [filters, setFilters] = useState({
        ED: true,
        NCED: true,
        GW: true,
        edges: true,
        labels: false
    });

    const attrs = filters.labels ? ["id","group","x","y","connectedTo","label"] : ["id","group","x","y","connectedTo"];
    const nodes = model.getNetwork(attrs).filter(node => 
        filters.ED && node.group === "ED" || filters.NCED && node.group === "NCED" || filters.GW && node.group === "GW"
    );
    const edges = nodes.filter(node => node.group === "ED" && filters.edges).map(node => ({
        id: generateRandomString(),
        from: node.id,
        to: node.connectedTo
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
    useEffect(()=>{
        netRef.current && new Network(netRef.current, networkData, options);
    }, [netRef, filters]);

    return (
        <Grid container spacing={2} alignItems="center">
            <Grid item xs={3}>
                <div style={{...styles.container, ...styles.form}}>
                    <h4 style={{padding:0, margin:0}}>Filtros de visualización</h4>
                    <FormControlLabel
                        control={
                            <Checkbox
                                checked={filters.ED}
                                onChange={e => setFilters({...filters, ED: e.target.checked})}/>
                        }
                        label="ED conectados"
                    />
                    <br />
                    <FormControlLabel
                        control={
                            <Checkbox
                                checked={filters.NCED}
                                onChange={e => setFilters({...filters, NCED: e.target.checked})}/>
                        }
                        label="ED Desconectados"
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
                                checked={filters.edges}
                                onChange={e => setFilters({...filters, edges: e.target.checked})}/>
                        }
                        label="Enlaces"
                    />
                    <br />
                    <FormControlLabel
                        control={
                            <Checkbox
                                checked={filters.labels}
                                onChange={e => setFilters({...filters, labels: e.target.checked})}/>
                        }
                        label="Etiquetas"
                    />
                </div>
            </Grid>
            <Grid item xs={9}>
                <div ref={netRef} style={styles.container}></div>
            </Grid>
        </Grid>
    );
};

export default LoRaNetwork;