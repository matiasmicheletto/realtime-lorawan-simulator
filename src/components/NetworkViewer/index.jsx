import React from 'react';
import { Checkbox, FormControlLabel, Grid, Box } from '@mui/material';

const styles = {
    container: {
        width:"100%", 
        height:"50vh",
        border: "1px solid #bbb",
        boxShadow: "3px 3px 10px gray",
        borderRadius: "1%"
    },
    form: {
        padding: "20px"
    }
}

const edgeColors = [];

const NetworkViewer = () => {

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
                        <FormControlLabel
                            control={
                                <Checkbox
                                    checked={false}
                                    onChange={()=>{}}/>
                            }
                            label="Labels"
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
                                    Object.keys(edgeColors).map( (sf, index) => (
                                        <FormControlLabel
                                            key={index}
                                            label={sf}
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
                    </div>
                </Grid>
                <Grid item xs={12} md={9}>
                    <div style={styles.container}></div>
                </Grid>
            </Grid>
        </div>
    );
};

export default NetworkViewer;