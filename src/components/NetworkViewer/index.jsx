import React, { useRef, useState, useEffect } from 'react';
import { Checkbox, FormControlLabel, Grid, Box, Button } from '@mui/material';

const styles = {
    container: {        
        border: "1px solid #bbb",
        boxShadow: "3px 3px 10px gray",
        borderRadius: "1%"
    },
    canvas : {        
        width: "100%",
    },
    form: {
        padding: "10px",
        height: "100%"
    }
};


const NetworkViewer = () => {

    const canvasRef = useRef(null);        
    let currentFrame = 0;

    useEffect(() => {
        const canvas = canvasRef.current;        
        canvas.width = window.innerWidth/2;
        canvas.height = canvas.width;
    }, []);

    const drawNetwork = () => {
        const canvas = canvasRef.current;
        const ctx = canvas.getContext('2d');
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        
        const scale = canvasRef.current.width;
        const iter = frames[currentFrame];

        const nlen = iter.nodes.length;
        const elen = iter.edges.length;
        let i = 0;
        while(i < nlen) {
            if(iter.nodes[i]){
                const {x, y, color} = iter.nodes[i];
                ctx.fillStyle = color;
                ctx.fillRect(x*scale, y*scale, 1, 1);
            }
            i++;
        }
        i = 0;
        while(i < elen) {
            if(iter.edges[i]){                
                const fromNode = iter.nodes[iter.edges[i].from];
                const toNode = iter.nodes[iter.edges[i].to];
                ctx.beginPath();
                ctx.moveTo(fromNode.x*scale, fromNode.y*scale);
                ctx.lineTo(toNode.x*scale, toNode.y*scale);
                ctx.strokeStyle = iter.edges[i].color;
                ctx.stroke();
            }
            i++;
        }
        
        if(currentFrame < frames.length-1){
            currentFrame++;
            setTimeout(drawNetwork, 100);
        }   
    };

    const repeatAnimation = () => {        
        currentFrame = 0;
        drawNetwork();
    };

    return (
        <div style={{marginTop: "20px"}}>
            <Grid container spacing={2} alignItems="center">
                <Grid item xs={12} md={3}>
                    <div style={{...styles.container, ...styles.form}}>
                        <h4 style={{padding:0, margin:0}}>Visualization filters</h4>
                        <FormControlLabel
                            control={
                                <Checkbox
                                    checked={false}
                                    onChange={()=>{}}/>
                            }
                            label="Connected ED"
                        />                        
                        <FormControlLabel
                            control={
                                <Checkbox
                                    checked={false}
                                    onChange={()=>{}}/>
                            }
                            label="Disconnected ED"
                        />                        
                        <FormControlLabel
                            control={
                                <Checkbox
                                    checked={false}
                                    onChange={()=>{}}/>
                            }
                            label="Gateways"
                        />                        
                        <div>
                            <FormControlLabel
                                control={
                                    <Checkbox
                                        checked={false}
                                        indeterminate={false}
                                        onChange={()=>{}} />
                                }
                                label="Links"
                            />
                            <Box sx={{ display: 'flex', flexDirection: 'column', ml: 3 }}>
                                {
                                    Object.keys(Module.edgeColors).map( (sf, index) => (
                                        <FormControlLabel
                                            key={index}
                                            label={"SF"+sf}
                                            control={
                                                <Checkbox 
                                                    checked={false} 
                                                    onChange={()=>{}} />
                                            }
                                        />
                                    ))
                                }    
                            </Box>
                        </div>
                        <Button
                            style={{width: "100%"}}
                            color="secondary"
                            variant="contained"
                            onClick={repeatAnimation}>
                            Repeat animation
                        </Button>
                    </div>
                </Grid>
                <Grid item xs={12} md={9}>
                    <div style={styles.container}>
                        <canvas style={styles.canvas} ref={canvasRef}></canvas>
                    </div>
                </Grid>
            </Grid>
        </div>
    );
};

export default NetworkViewer;