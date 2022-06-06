import React, { useContext } from 'react';
import { Grid, LinearProgress } from '@mui/material';
import { ModelContext, LoadingContext } from '../../context';

const styles = {
    form: {
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
    const { model } = useContext(ModelContext);
    const { loading } = useContext(LoadingContext);

    const {
        status,
        iteration,
        exitCondition,
        elapsed,
        ufAvg,
        coverage
    } = model;
    
    return (                
        <div style={styles.form}>
            { loading && <LinearProgress style={{marginTop: "20px", marginBottom: "20px"}}/>}
            <Grid container spacing={1}>
                <Grid item xs={6}>
                    <span><b>Status: </b></span> <span style={{color:status==="running" ? "red" : "green"}}>{status}</span>
                </Grid>
                <Grid item xs={6}>
                    <span><b>Iteration: </b></span> <span>{iteration}</span>
                </Grid>
                <Grid item xs={6}>
                    <span><b>Exit condition: </b></span> <span style={{color:exitCondition === "timeout" ? "red" : "green"}}>{exitCondition === "" ? "-" : exitCondition}</span>
                </Grid>
                <Grid item xs={6}>
                    <span><b>Elapsed time: </b></span> <span>{elapsed === 0 ? "-" : (elapsed/1000)?.toFixed(2)+" s"}</span>
                </Grid>
                <Grid item xs={6}>
                    <span><b>Average UF: </b></span> <span>{(ufAvg*100)?.toFixed(2)} %</span>
                </Grid>
                <Grid item xs={6}>
                    <span><b>Network coverage: </b></span> <span>{coverage?.toFixed(2)} %</span>
                </Grid>
            </Grid>
        </div>
    )
};

export default OutputBlock;