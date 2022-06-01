import React, { useRef, useState, useContext } from 'react';
import { 
    Grid, 
    TextField, 
    Button, 
    Select, 
    MenuItem, 
    FormControl, 
    InputLabel 
} from '@mui/material';
import { ModelCtx } from '../../context';
import { readFile } from '../../utils';

const styles = {
    button: {
        width:"100%"
    },
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

const ConfigForm = () => {

    const fileInputRef = useRef(null);
    const [format, setFormat] = useState("csv");

    const model = useContext(ModelCtx);

    const [inputs, setInputs] = useState(model.getAllParams());
    const {
        N,
        H,
        mapWidth,
        mapHeight,
        posDistr,
        periodsDistr,
        initialGW,
        strategy,
        maxIter,
        maxRuntime,
        updateFreq
    } = inputs;

    const handleImportModel = () => {
        fileInputRef.current.click();
    };

    const handleLoadFile = e => {
        if(e.target.files?.length > 0)
            readFile(e.target.files[0], format)
            .then(result => model.importModel(result, format)
                .then(() => 
                    setInputs(model.getAllParams()
                ))
            );
    };

    const handleExportModel = () => {
        const data = model.exportModel(format);
        const extensions = {
            csv: "csv",
            json: "json",
            matlab: "m"
        };
        var blob = new Blob([data], { type: 'text/plain' });
        var a = document.createElement('a');
        a.download = `NetworkModel.${extensions[format]}`;
        a.href = window.URL.createObjectURL(blob);
        a.click();
    };

    const handleFormatChange = e => {        
        setFormat(e.target.value);
    };

    const runSimulation = () => {
        model.run().then(results => console.log(results));
    };

    return (        
        <div style={styles.form}>
            <Grid container spacing={2} alignItems="center">
                <Grid item xs={4}>
                    <Button 
                        style={styles.button}
                        variant="contained"
                        color="error"
                        onClick={handleImportModel}>
                        Import configuration
                    </Button>
                    <TextField                        
                        type="file"
                        style={{display:"none"}}
                        inputRef={fileInputRef}
                        onChange={handleLoadFile}/>  
                </Grid>
                <Grid item xs={4}>
                    <Button 
                        style={styles.button}
                        color="info"
                        variant="contained"
                        onClick={handleExportModel}>
                        Export network
                    </Button>
                </Grid>
                <Grid item xs={4}>    
                    <FormControl fullWidth>
                        <InputLabel>Format</InputLabel>
                        <Select                        
                            value={format}
                            label="Format"
                            onChange={handleFormatChange}>
                            <MenuItem value="csv">CSV</MenuItem>
                            <MenuItem value="json">JSON</MenuItem>
                            <MenuItem value="matlab">MATLAB</MenuItem>
                        </Select>
                    </FormControl>
                </Grid>
            </Grid>
            <Grid container spacing={2} style={{marginTop:"10px"}}>
                <Grid item xs={6}>
                    <TextField
                        style={{width: "100%"}}
                        value={N}
                        variant="outlined"
                        type="number"
                        size="small"
                        label="Number of end devices"
                        inputProps={{min:0}}/>
                </Grid>
                <Grid item xs={6}>
                    <TextField
                        style={{width: "100%"}} 
                        value={H}
                        variant="outlined"
                        type="number"
                        size="small"
                        label="System hyperperiod"
                        inputProps={{min:10}}/>
                </Grid>
            </Grid>
            <Grid container spacing={2} style={{marginTop:"10px"}}>
                <Grid item xs={6}>
                    <TextField
                        style={{width: "100%"}}
                        value={mapWidth}
                        variant="outlined"
                        type="number"
                        size="small"
                        label="Map width (mts)"
                        inputProps={{min:0}}/>
                </Grid>
                <Grid item xs={6}>
                    <TextField
                        style={{width: "100%"}} 
                        value={mapHeight}
                        variant="outlined"
                        type="number"
                        size="small"
                        label="Map height (mts)"
                        inputProps={{min:10}}/>
                </Grid>
            </Grid>
            <Grid container spacing={2} style={{marginTop:"10px"}}>
                <Grid item xs={6}>
                    <FormControl fullWidth>
                        <InputLabel id="dist-select-label">End devices dist.</InputLabel>
                        <Select                        
                            labelId="dist-select-label"
                            value={posDistr}
                            label="End device dist.">
                            <MenuItem value="uniform">Uniform</MenuItem>
                            <MenuItem value="gauss">Gaussian</MenuItem>
                            <MenuItem value="gauss">Clouds</MenuItem>
                        </Select>
                    </FormControl>
                </Grid>
                <Grid item xs={6}>
                    <TextField
                        style={{width: "100%"}}
                        value={periodsDistr}
                        variant="outlined"
                        type="text"
                        label="Period distribution"/>
                </Grid>
            </Grid>
            <Grid container spacing={2} style={{marginTop:"10px"}}>
                <Grid item xs={6}>
                    <TextField
                        style={{width: "100%"}} 
                        value={initialGW}
                        variant="outlined"
                        type="number"
                        label="Initial gateway number"
                        inputProps={{min:0}}/>
                </Grid>
                <Grid item xs={6}>
                    <FormControl fullWidth>
                        <InputLabel id="strat-select-label">Gateway positioning strategy</InputLabel>
                        <Select                        
                            labelId="strat-select-label"
                            value={strategy}
                            label="Gateway positioning strategy">
                            <MenuItem value="random">Random</MenuItem>
                            <MenuItem value="strings">Springs</MenuItem>
                        </Select>
                    </FormControl>
                </Grid>
            </Grid>
            <Grid container spacing={2} style={{marginTop:"10px"}}>
                <Grid item xs={4}>
                    <TextField
                        style={{width: "100%"}} 
                        value={maxIter}
                        variant="outlined"
                        type="number"
                        label="Max. iterations"
                        inputProps={{min:0}}/>
                </Grid>
                <Grid item xs={4}>
                    <TextField
                        style={{width: "100%"}} 
                        value={maxRuntime}
                        variant="outlined"
                        type="number"
                        label="Max. execution time (s)"
                        inputProps={{min:0}}/>
                </Grid>
                <Grid item xs={4}>
                    <FormControl fullWidth>
                        <InputLabel id="vis-select-label">Update chart</InputLabel>
                        <Select                        
                            labelId="vis-select-label"
                            value={updateFreq}
                            label="Update chart">
                            <MenuItem value="1">after each iteration</MenuItem>
                            <MenuItem value="10">after 10 iterations</MenuItem>
                            <MenuItem value="1000">after 1000 iterations</MenuItem>
                            <MenuItem value="end">on finish</MenuItem>
                        </Select>
                    </FormControl>
                </Grid>
            </Grid>
            <Grid container style={{marginTop:"10px"}} justifyContent="flex-end">                
                <Button
                    color="primary"
                    variant="contained"
                    onClick={runSimulation}>
                    Run simulation
                </Button>
            </Grid>
        </div>
    );
};

export default ConfigForm;
