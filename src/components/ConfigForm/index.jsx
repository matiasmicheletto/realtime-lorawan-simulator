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
import { LoadingContext, manager } from '../../context';
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

    const { loading, setLoading } = useContext(LoadingContext);

    const [inputs, setInputs] = useState(manager.getAllParams());
    const {
        N,
        H,
        mapWidth,
        mapHeight,
        posDistr,
        periodsDistr,
        initialGW,
        strategy,
        schedulingBy,
        maxIter,
        addGWAfter,
        maxRuntime,
        updateRate
    } = inputs;

    const handleImportModel = () => {
        fileInputRef.current.click();
    };

    const handleLoadFile = e => {
        if(e.target.files?.length > 0)
            readFile(e.target.files[0], format)
            .then(result => {
                try{
                    manager.importConfig(result, format);
                    setInputs(manager.getAllParams());
                }catch(e){
                    console.error(e);
                }
            });
    };

    const handleExportModel = () => {
        const data = manager.exportModel(format);
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
        if(!loading){
            const { name, value } = e.target;
            console.log(name, value);
            manager.configure({ [name]: value });
            if(["N", "mapWidth", "mapHeight", "posDistr", "initialGW", "strategy", "schedulingBy", "periodsDistr"].includes(name)){ // Only update network for certain parameters
                manager.initialize();
            }
            setInputs(prev => ({ ...prev, [name]: value }));
        }
    };

    const runSimulation = () => {
        if(!loading){
            setLoading(true);
            manager.start().then(results => {
                setLoading(false);
                console.log(results);
            });
        }
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
                    <Grid item xs={12} md={6}>
                        <TextField
                            style={{width: "100%"}}
                            value={N}
                            variant="outlined"
                            type="number"
                            size="small"
                            label="Number of end devices"
                            name="N"
                            onChange={handleInputChange}
                            inputProps={{min:1,max:15000}}/>
                    </Grid>
                    <Grid item xs={12} md={6}>
                        <TextField
                            style={{width: "100%"}} 
                            value={H}
                            variant="outlined"
                            type="number"
                            size="small"
                            label="System hyperperiod"
                            name="H"
                            onChange={handleInputChange}
                            inputProps={{min:1, max:10000}}/>
                    </Grid>
                </Grid>
                <Grid container spacing={2} style={{marginTop:"10px"}}>
                    <Grid item xs={12} md={6}>
                        <TextField
                            style={{width: "100%"}}
                            value={mapWidth}
                            variant="outlined"
                            type="number"
                            size="small"
                            label="Map width (mts)"
                            name="mapWidth"
                            onChange={handleInputChange}
                            inputProps={{min:1,max:10000}}/>
                    </Grid>
                    <Grid item xs={12} md={6}>
                        <TextField
                            style={{width: "100%"}} 
                            value={mapHeight}
                            variant="outlined"
                            type="number"
                            size="small"
                            label="Map height (mts)"
                            name="mapHeight"
                            onChange={handleInputChange}
                            inputProps={{min:1,max:10000}}/>
                    </Grid>
                    <Grid item xs={12} md={6}>
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
                    <Grid item xs={12} md={6}>
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
                    <Grid item xs={12} md={4} lg={3}>
                        <TextField
                            style={{width: "100%"}} 
                            value={initialGW}
                            variant="outlined"
                            type="number"
                            label="Initial gateway number"
                            name="initialGW"
                            onChange={handleInputChange}
                            inputProps={{min:1,max:16}}/>
                    </Grid>
                    <Grid item xs={12} md={4} lg={3}>
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
                    <Grid item xs={12} md={4} lg={3}>
                        <FormControl fullWidth>
                            <InputLabel id="addgw-select-label">Add GW on low cov.</InputLabel>
                            <Select                        
                                labelId="addgw-select-label"
                                value={addGWAfter}
                                label="Add GW on low cov."
                                name="addGWAfter"
                                onChange={handleInputChange}>
                                <MenuItem value={25}>after 25 iterations</MenuItem>
                                <MenuItem value={50}>after 50 iterations</MenuItem>
                                <MenuItem value={100}>after 100 iterations</MenuItem>
                                <MenuItem value={1000}>after 100 iterations</MenuItem>
                            </Select>
                        </FormControl>
                    </Grid>
                    <Grid item xs={12} md={4} lg={3}>
                        <FormControl fullWidth>
                            <InputLabel id="strat-select-label">Scheduling method</InputLabel>
                            <Select                        
                                labelId="strat-select-label"
                                value={schedulingBy}
                                label="Scheduling method"
                                name="schedulingBy"
                                onChange={handleInputChange}>
                                <MenuItem value="gw">By gateway</MenuItem>
                                <MenuItem value="ed">By end device</MenuItem>
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
                            inputProps={{min:1,max:1000}}/>
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
                            inputProps={{min:10,max:300}}/>
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
        </div>
    );
};

export default ConfigForm;
