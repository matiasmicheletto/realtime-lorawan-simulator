import React, { useState } from 'react';
import { Grid, LinearProgress } from '@mui/material';

const styles = {
    form: {
        lineHeight: "0.9em",
        border: "1px solid #bbb",
        padding: "10px",
        borderRadius: "5px",
        boxShadow: "3px 3px 10px gray",
        marginTop: "10px",
        paddingTop: "30px",
        paddingBottom: "30px"
    }
};
const OutputBlock = () => {
   
    const [ outputs, setOutputs ] = useState({
        iters: 0,
        elapsed: 0,
        exitCode: "",
        gws: 0,
        coverage: 0,
        channels: 0
    });
    Module.onResults = setOutputs;

    const { iters, elapsed, exitCode, gws, coverage, channels } = outputs;

    return (                
        <div style={styles.form}>
            <Grid container spacing={3}>
                <Grid item xs={12} md={6} lg={3}>
                    <b>Iterations:</b> <span>{iters}</span>
                </Grid>
                <Grid item xs={12} md={6} lg={3}>
                    <b>Elapsed time:</b> <span>{elapsed} ms</span>
                </Grid>
                <Grid item xs={12} md={6} lg={3}>
                    <b>Exit condition:</b> <span>{exitCode}</span>
                </Grid>
                <Grid item xs={12} md={6} lg={3}>
                    <b>GW used:</b> <span>{gws}</span>
                </Grid>
                <Grid item xs={12} md={6} lg={3}>
                    <b>Coverage:</b> <span>{coverage*100} %</span>
                </Grid>
                <Grid item xs={12} md={6} lg={3}>
                    <b>Channels:</b> <span>{channels}</span>
                </Grid>
            </Grid>
        </div>
    )
};

export default OutputBlock;