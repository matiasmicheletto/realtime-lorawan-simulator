import React, { useRef, useState, useContext } from 'react';
import { 
    Grid, 
    TextField, 
    Button, 
    Select, 
    MenuItem, 
    FormControl, 
    InputLabel,
    LinearProgress 
} from '@mui/material';
import NetworkViewer from '../NetworkViewer';
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
    const [loading, setLoading] = useState(false);
    const [updCnt, setUpdCnt] = useState(0);

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
        updateRate
    } = inputs;

    const [outputs, setOutputs] = useState({
        status: "ready", // ready, running, not-initialized
        exitCondition: "",
        elapsed: 0,
        ufAvg: 0,
        coverage: 0
    });
    const {
        status,
        exitCondition,
        elapsed,
        ufAvg,
        coverage
    } = outputs;

    model.onChange = res => {
        setOutputs(res); 
        setUpdCnt(updCnt + 1);
    };

    const handleImportModel = () => {
        fileInputRef.current.click();
    };

    const handleLoadFile = e => {
        if(e.target.files?.length > 0)
            readFile(e.target.files[0], format)
            .then(result => {
                try{
                    model.importModel(result, format);
                    setInputs(model.getAllParams());
                }catch(e){
                    console.error(e);
                }
            });
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

    const handleInputChange = e => {
        const { name, value } = e.target;
        console.log(name, value);
        model.configure({ [name]: value });
        if(["N", "mapWidth", "mapHeight", "posDistr", "initialGW"].includes(name)){ // Only update network for certain parameters
            model.initialize();
        }
        setInputs(prev => ({ ...prev, [name]: value }));
    }

    const runSimulation = () => {
        setLoading(true);
        model.start().then(results => {
            setLoading(false);
            console.log(results);
        });
    };

    return (        
        <div>
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
                            name="N"
                            onChange={handleInputChange}
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
                            name="H"
                            onChange={handleInputChange}
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
                            name="mapWidth"
                            onChange={handleInputChange}
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
                            name="mapHeight"
                            onChange={handleInputChange}
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
                                name="posDistr"
                                onChange={handleInputChange}
                                label="End device dist.">
                                <MenuItem value="uniform">Uniform</MenuItem>
                                <MenuItem value="normal">Normal</MenuItem>
                                <MenuItem value="clouds">Clouds</MenuItem>
                            </Select>
                        </FormControl>
                    </Grid>
                    <Grid item xs={6}>
                        <TextField
                            style={{width: "100%"}}
                            value={periodsDistr}
                            variant="outlined"
                            type="text"
                            label="Period distribution"
                            name="periodsDistr"
                            onChange={handleInputChange}/>
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
                            name="initialGW"
                            onChange={handleInputChange}
                            inputProps={{min:0}}/>
                    </Grid>
                    <Grid item xs={6}>
                        <FormControl fullWidth>
                            <InputLabel id="strat-select-label">Gateway positioning strategy</InputLabel>
                            <Select                        
                                labelId="strat-select-label"
                                value={strategy}
                                label="Gateway positioning strategy"
                                name="strategy"
                                onChange={handleInputChange}>
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
                            name="maxIter"
                            onChange={handleInputChange}
                            inputProps={{min:0}}/>
                    </Grid>
                    <Grid item xs={4}>
                        <TextField
                            style={{width: "100%"}} 
                            value={maxRuntime}
                            variant="outlined"
                            type="number"
                            label="Max. execution time (s)"
                            name="maxRuntime"
                            onChange={handleInputChange}
                            inputProps={{min:0}}/>
                    </Grid>
                    <Grid item xs={4}>
                        <FormControl fullWidth>
                            <InputLabel id="vis-select-label">Update chart</InputLabel>
                            <Select                        
                                labelId="vis-select-label"
                                value={updateRate}
                                label="Update chart"
                                name="updateRate"
                                onChange={handleInputChange}>
                                <MenuItem value={1}>after each iteration</MenuItem>
                                <MenuItem value={10}>after 10 iterations</MenuItem>
                                <MenuItem value={100}>after 100 iterations</MenuItem>
                                <MenuItem value={0.1}>on finish</MenuItem>
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
            { loading && <LinearProgress style={{marginTop: "20px", marginBottom: "20px"}}/>}
                
            <div style={styles.form}>
                <span><b>Status: </b></span> <span style={{color:status==="running" ? "red" : "green"}}>{status}</span>
                <br />
                <span><b>Exit condition: </b></span> <span style={{color:exitCondition === "timeout" ? "red" : "green"}}>{exitCondition === "" ? "-" : exitCondition}</span>
                <br />
                <span><b>Elapsed time: </b></span> <span>{elapsed === 0 ? "-" : (elapsed/1000).toFixed(2)+" s"}</span>
                <br />
                <span><b>Average UF: </b></span> <span>{(ufAvg*100).toFixed(2)} %</span>
                <br />
                <span><b>Network coverage: </b></span> <span>{coverage.toFixed(2)} %</span>
            </div>

            <NetworkViewer model={model} update={updCnt}/>
        </div>
    );
};

export default ConfigForm;
