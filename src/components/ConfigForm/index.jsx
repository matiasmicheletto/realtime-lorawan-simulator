import React, {useState} from 'react';
import { 
    Grid, 
    TextField, 
    Button, 
    Select, 
    MenuItem, 
    FormControl, 
    InputLabel 
} from '@mui/material';

const styles = {
    button: {
        width:"100%"
    },
    container: {
        border: "1px solid #bbb",
        padding: "10px",
        borderRadius: "5px",
        boxShadow: "3px 3px 10px gray",
        marginTop: "20px",
        paddingTop: "10px",
        paddingBottom: "30px"        
    }
};

const ConfigForm = () => {

    const [inputs, setInputs] = useState({
        mapSize: "1000",
        edNumber: "10000",
        initialGW: "1",
        maxSF: "10",
        posDist: "0",
        periodDist: "0",
        maxIter: "1000",
        timeout: "360",
        algorithm: "0",
        updateRate: "10"
    });

    const {
        mapSize,
        edNumber,
        initialGW,
        maxSF,
        posDist,
        periodDist,
        maxIter,
        timeout,
        algorithm,
        updateRate
    } = inputs;

    const handleInputChange = event => {
        const {name, value} = event.target;
        setInputs({...inputs, [name]: value});
    };

    const runSimulation = () => {
        if(optimizerReady){      
            Module.frames = [];        
            Module._runSimulation(
                mapSize,
                edNumber,
                initialGW,
                maxSF,
                posDist,
                periodDist,
                maxIter,
                timeout,
                algorithm,
                updateRate
            );
        }
    }

    return (        
        <div style={styles.container}>
            <h3>Instance configuration</h3>
            <Grid container spacing={2}>
                <Grid item xs={12} md={6} lg={3}>
                    <TextField
                        style={{width: "100%"}}
                        value={mapSize}
                        variant="outlined"
                        type="number"
                        size="small"
                        label="Map size"
                        name="mapSize"
                        onChange={handleInputChange}
                        inputProps={{min:10,max:10000}}/>
                </Grid>
                <Grid item xs={12} md={6} lg={3}>
                    <TextField
                        style={{width: "100%"}}
                        value={edNumber}
                        variant="outlined"
                        type="number"
                        size="small"
                        label="Number of end devices"
                        name="edNumber"
                        onChange={handleInputChange}
                        inputProps={{min:1,max:100000}}/>
                </Grid>
                <Grid item xs={12} md={6} lg={3}>
                    <TextField
                        style={{width: "100%"}}
                        value={initialGW}
                        variant="outlined"
                        type="number"
                        size="small"
                        label="Initial gateways"
                        name="initialGW"
                        onChange={handleInputChange}
                        inputProps={{min:1,max:100000}}/>
                </Grid>
                <Grid item xs={12} md={6} lg={3}>
                    <FormControl fullWidth>
                        <InputLabel>Max. SF</InputLabel>
                        <Select                        
                            value={maxSF}
                            label="Max. SF"
                            size="small"
                            name="maxSF"
                            onChange={handleInputChange}>
                            <MenuItem value="7">SF7</MenuItem>
                            <MenuItem value="8">SF8</MenuItem>
                            <MenuItem value="9">SF9</MenuItem>
                            <MenuItem value="10">SF10</MenuItem>
                            <MenuItem value="11">SF11</MenuItem>
                            <MenuItem value="12">SF12</MenuItem>
                        </Select>
                    </FormControl>
                </Grid>
                <Grid item xs={12} md={6} lg={3}>
                    <FormControl fullWidth>
                        <InputLabel>Pos. Dist.</InputLabel>
                        <Select                        
                            value={posDist}
                            label="Pos. Dist."
                            size="small"
                            name="posDist"
                            onChange={handleInputChange}>
                            <MenuItem value="0">Uniform</MenuItem>
                            <MenuItem value="1">Normal</MenuItem>
                            <MenuItem value="2">Clouds</MenuItem> 
                        </Select>
                    </FormControl>
                </Grid>
                <Grid item xs={12} md={6} lg={3}>
                    <FormControl fullWidth>
                        <InputLabel>Period Dist.</InputLabel>
                        <Select                        
                            value={periodDist}
                            label="Period Dist."
                            size="small"
                            name="periodDist"
                            onChange={handleInputChange}>
                            <MenuItem value="0">Soft</MenuItem>                            
                            <MenuItem value="1">Medium</MenuItem>
                            <MenuItem value="2">Hard</MenuItem>
                        </Select>
                    </FormControl>
                </Grid>
                <Grid item xs={12} md={6} lg={3}>
                    <TextField
                        style={{width: "100%"}}
                        value={maxIter}
                        variant="outlined"
                        type="number"
                        size="small"
                        label="Maximum iterations"
                        name="maxIter"
                        onChange={handleInputChange}
                        inputProps={{min:1,max:10000}}/>
                </Grid>
                <Grid item xs={12} md={6} lg={3}>
                    <TextField
                        style={{width: "100%"}}
                        value={timeout}
                        variant="outlined"
                        type="number"
                        size="small"
                        label="Timeout"
                        name="timeout"
                        onChange={handleInputChange}
                        inputProps={{min:1,max:3600}}/>
                </Grid>
                <Grid item xs={12} md={6} lg={3}>
                    <FormControl fullWidth>
                        <InputLabel>Step algo.</InputLabel>
                        <Select                        
                            value={algorithm}
                            label="Step algo."
                            size="small"
                            name="algorithm"
                            onChange={handleInputChange}>
                            <MenuItem value="0">Springs</MenuItem>                            
                            <MenuItem value="1">Random</MenuItem>                            
                        </Select>
                    </FormControl>
                </Grid>
                <Grid item xs={12} md={6} lg={3}>
                    <FormControl fullWidth>
                        <InputLabel>Frame rate</InputLabel>
                        <Select                        
                            value={updateRate}
                            label="Frame rate"
                            size="small"
                            name="updateRate"
                            onChange={handleInputChange}>
                            <MenuItem value="1">Every iteration</MenuItem>                            
                            <MenuItem value="10">Every 10 iterations</MenuItem>
                            <MenuItem value="25">Every 25 iterations</MenuItem>
                            <MenuItem value="50">Every 50 iterations</MenuItem>
                            <MenuItem value="100">Every 100 iterations</MenuItem>
                        </Select>
                    </FormControl>
                </Grid>
            </Grid>

            <Grid container spacing={2} style={{marginTop:"10px"}} justifyContent="flex-start">
                <Grid item xs={6} md={3}>
                    <Button
                        style={{width: "100%", marginRight: "5px"}}
                        color="primary"
                        variant="contained"
                        onClick={runSimulation}>
                        Run instance
                    </Button>
                </Grid>
            </Grid>
        </div>
    );
};

export default ConfigForm;
