import React, { useRef, useState } from 'react';
import { Checkbox, FormControlLabel, Grid, Box, Button } from '@mui/material';

const styles = {
    container: {        
        padding: "10px",
        marginTop: "10px",
        border: "1px solid #bbb",
        boxShadow: "3px 3px 10px gray",
        borderRadius: "1%"
    },
    canvas : {        
        border: "1px solid #bbb",
        width: "100%"
    },
    form: {    
        padding: "10px",
        height: "100%"
    }
};

const NetworkViewer = () => {

    const canvasRef = useRef(null);        

    const [currentFrame, setCurrentFrame] = useState(-1);

    const [filters, setFilters] = useState({
        connected: true,
        disconnected: true,
        gateways: true
    });    

    const [edgesFilter, setEdgesFilter] = useState([
        true, // SF7
        true, // SF8
        true, // SF9
        true, // SF10
        true, // SF11
        true, // SF12
    ]);

    const toggleSingleEdge = (idx, checked) => {
        let newEdgesFilter = [...edgesFilter];
        newEdgesFilter[idx] = checked;
        setEdgesFilter(newEdgesFilter);        
    };

    const toggleEdges = checked => {
        setEdgesFilter(new Array(6).fill(checked));        
    };

    const canvas = canvasRef.current;

    if(canvas){

        canvas.width = window.innerWidth/2;
        canvas.height = canvas.width;
        
        const ctx = canvas.getContext('2d');
        const scale = canvas.width;
        const iter = Module.frames[currentFrame];

        if(iter){ 

            ctx.clearRect(0, 0, canvas.width, canvas.height);

            let i = 0;
            while(i < iter.nodes.length) {
                if(iter.nodes[i]){
                    const {x, y, color, group} = iter.nodes[i];
                    if(
                        group === 0 && filters.disconnected || 
                        group === 1 && filters.connected || 
                        group === 2 && filters.gateways
                    ){
                        ctx.fillStyle = color;
                        ctx.fillRect(x*scale, y*scale, 1, 1);
                    }
                }
                i++;
            }
            i = 0;
            while(i < iter.edges.length) {
                if(iter.edges[i]){
                    const fromNode = iter.nodes[iter.edges[i].from];
                    const toNode = iter.nodes[iter.edges[i].to];
                    const sf = iter.edges[i].sf;
                    if(edgesFilter[sf-7] && filters.gateways){
                        ctx.beginPath();
                        ctx.moveTo(fromNode.x*scale, fromNode.y*scale);
                        ctx.lineTo(toNode.x*scale, toNode.y*scale);
                        ctx.strokeStyle = iter.edges[i].color;
                        ctx.stroke();
                    }
                }
                i++;
            }

            ctx.fillStyle = "white";
            ctx.fillRect(5, 5, 135, 80);
            ctx.fillStyle = "black";
            ctx.font = "12px Arial bold";
            ctx.fillText(`Iteration: ${iter.iter}`, 10, 20);
            ctx.fillText(`Gateways: ${iter.gw}`, 10, 35);
            ctx.fillText(`Max. SF: ${iter.maxSF}`, 10, 50);
            ctx.fillText(`Connected EDs: ${iter.ced}`, 10, 65);
            ctx.fillText(`Not connected EDs: ${iter.nced}`, 10, 80);
            
            if(currentFrame < Module.frames.length-1)
                setTimeout(()=>setCurrentFrame(fr => fr < Module.frames.length-1 ? fr+1 : fr), 100);
        }      
    }   

    return (
        <div style={styles.container}>
            <h3>Progress animation</h3>
            <Grid container spacing={2} alignItems="stretch">
                <Grid item xs={12} md={3}>
                    <div style={styles.form}>
                        <h4 style={{padding:0, margin:0}}>Visualization filters</h4>
                        <FormControlLabel disabled={currentFrame < Module.frames.length-1}
                            control={
                                <Checkbox
                                    checked={filters.connected}
                                    onChange={e => {setFilters({...filters, connected: e.target.checked})}}/>
                            }
                            label="Connected ED"
                        />                        
                        <FormControlLabel disabled={currentFrame < Module.frames.length-1}
                            control={
                                <Checkbox
                                    checked={filters.disconnected}
                                    onChange={e => {setFilters({...filters, disconnected: e.target.checked})}}/>
                            }
                            label="Disconnected ED"
                        />                        
                        <FormControlLabel disabled={currentFrame < Module.frames.length-1}
                            control={
                                <Checkbox
                                    checked={filters.gateways}
                                    onChange={e => {setFilters({...filters, gateways: e.target.checked})}}/>
                            }
                            label="Gateways"
                        />                        
                        <div>
                            <FormControlLabel disabled={currentFrame < Module.frames.length-1}
                                control={
                                    <Checkbox
                                        checked={edgesFilter.every(f => f)}
                                        indeterminate={edgesFilter.some(f => f) && !edgesFilter.every(f => f)}
                                        onChange={e => toggleEdges(e.target.checked)} />
                                }
                                label="Links"
                            />
                            <Box sx={{ display: 'flex', flexDirection: 'column', ml: 3 }}>
                                {
                                    Object.keys(Module.edgeColors).map( (sf, index) => (
                                        <FormControlLabel disabled={currentFrame < Module.frames.length-1}
                                            key={index}
                                            label={"SF"+sf}
                                            control={
                                                <Checkbox 
                                                    checked={edgesFilter[index]} 
                                                    onChange={e => toggleSingleEdge(index, e.target.checked)} />
                                            }
                                        />
                                    ))
                                }    
                            </Box>
                        </div>
                        <Button
                            disabled={currentFrame < Module.frames.length-1}
                            style={{width: "100%", marginTop: "10px"}}
                            variant="contained"
                            onClick={()=>setCurrentFrame(0)}>
                            Run animation
                        </Button>
                    </div>
                </Grid>
                <Grid item xs={12} md={9}>
                    <div>
                        <canvas style={styles.canvas} ref={canvasRef}></canvas>
                    </div>
                </Grid>
            </Grid>
        </div>
    );
};

export default NetworkViewer;