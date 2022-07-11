import React, { useRef, useEffect } from 'react';
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
    let currentFrame = 0;

    useEffect(() => {
        const canvas = canvasRef.current;        
        canvas.width = window.innerWidth/2;
        canvas.height = canvas.width;
    }, []);

    const drawNetwork = () => {
        const canvas = canvasRef.current;
        const ctx = canvas.getContext('2d');
        const scale = canvas.width;
        const iter = Module.frames[currentFrame];
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        
        let i = 0;
        while(i < iter.nodes.length) {
            if(iter.nodes[i]){
                const {x, y, color} = iter.nodes[i];
                ctx.fillStyle = color;
                ctx.fillRect(x*scale, y*scale, 1, 1);
            }
            i++;
        }
        i = 0;
        while(i < iter.edges.length) {
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

        ctx.fillStyle = "white";
        ctx.fillRect(5, 5, 135, 80);
        ctx.fillStyle = "black";
        ctx.font = "12px Arial bold";
        ctx.fillText(`Iteration: ${iter.iter}`, 10, 20);
        ctx.fillText(`Gateways: ${iter.gw}`, 10, 35);
        ctx.fillText(`Max. SF: ${iter.maxSF}`, 10, 50);
        ctx.fillText(`Connected EDs: ${iter.ced}`, 10, 65);
        ctx.fillText(`Not connected EDs: ${iter.nced}`, 10, 80);
        
        if(currentFrame < Module.frames.length-1){
            currentFrame++;
            setTimeout(drawNetwork, 100);
        }else{
            currentFrame = 0;
        }   
    };

    return (
        //Module.frames.length > 0 ?
            <div style={styles.container}>
                <h3>Progress animation</h3>
                <Grid container spacing={2} alignItems="stretch">
                    <Grid item xs={12} md={3}>
                        <div style={styles.form}>
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
                                style={{width: "100%", marginTop: "10px"}}
                                variant="contained"
                                onClick={drawNetwork}>
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
            //:
            //null
    );
};

export default NetworkViewer;