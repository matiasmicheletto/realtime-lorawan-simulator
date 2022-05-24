import React, { useRef, useState } from 'react';
import { 
    Grid, 
    TextField, 
    Button, 
    Select, 
    MenuItem, 
    FormControl, 
    InputLabel 
} from '@mui/material';
import MainView from '../../templates/MainView';
import { readFile } from '../../utils';

const styles = {
    button: {
        width:"100%"
    },
    form: {
        border: "1px solid #ccc",
        padding: "10px",
        borderRadius: "5px",
        marginTop: "10px"
    }
};

const Config = () => {

    const fileInputRef = useRef(null);
    const [format, setFormat] = useState("csv");

    const handleOpenFile = () => {
        fileInputRef.current.click();
    };

    const handleLoadFile = e => {
        if(e.target.files?.length > 0)
            readFile(e.target.files[0], format)
            .then(result => console.log(result));
    };

    const handleFormatChange = e => {        
        setFormat(e.target.value);
    };

    return (
        <MainView>
            <h3>Network configuration</h3>
            <div style={styles.form}>
                <Grid container spacing={2} alignItems="center">
                    <Grid item xs={4}>
                        <Button 
                            style={styles.button}
                            variant="contained"
                            color="error"
                            onClick={handleOpenFile}>
                            Import
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
                            onClick={handleOpenFile}>
                            Export
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
                        </Select>
                        </FormControl>
                    </Grid>
                </Grid>
                <Grid container spacing={2}>
                    <Grid item xs={12}>
                        <TextField
                            variant="outlined"
                            type="number"
                            size="small"                            
                            label="End Devices number"
                            inputProps={{min:0}}/>
                    </Grid>
                    <Grid item xs={12}>
                        <TextField
                            variant="outlined"
                            type="number"
                            size="small"
                            label="Hyperperiod"
                            inputProps={{min:10}}/>
                    </Grid>
                </Grid>
            </div>
        </MainView>
    );
};

export default Config;
