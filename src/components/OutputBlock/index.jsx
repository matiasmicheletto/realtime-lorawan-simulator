import React from 'react';
import { Grid, LinearProgress } from '@mui/material';

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
   
    return (                
        <div style={styles.form}>
        </div>
    )
};

export default OutputBlock;