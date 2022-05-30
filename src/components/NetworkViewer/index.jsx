import React, { useContext, useEffect, useRef } from 'react';
import { visLinkAttrs, visNodeAttrs } from 'lora-network-model/network-model';
import { Network } from 'vis-network';
import { DataSet } from 'vis-data';
import { ModelCtx } from '../../context';
import classes from './style.module.css';

const LoRaNetwork = () => {

    const netRef = useRef(null);

    const model = useContext(ModelCtx);

    const data = {
        nodes: new DataSet(model.getAllNodes(visNodeAttrs)),
        edges: new DataSet(model.getLinks(visLinkAttrs)),
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
        netRef.current && new Network(netRef.current, data, options);
    }, [netRef, data, options]);

    return (
        <div ref={netRef} className={classes.Container}>
        </div>
    );
};

export default LoRaNetwork;