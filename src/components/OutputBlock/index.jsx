import React, { useState, useContext } from 'react';
import { LoadingContext } from '../../context/loading';
import { Table, TableHead, TableBody, TableRow, TableCell } from '@mui/material';

const styles = {
    container: {
        border: "1px solid #bbb",
        padding: "10px",
        borderRadius: "5px",
        boxShadow: "3px 3px 10px gray",
        marginTop: "10px",
        paddingTop: "10px",
        paddingBottom: "30px"
    }
};
const OutputBlock = () => {
    const [ outputs, setOutputs ] = useState([]);
    const {setLoading} = useContext(LoadingContext);

    Module.onResultsUpdate = (iters,elapsed,exitCode,gws,coverage,channels) => {
        const temp = [...outputs];
        temp.push({iters,elapsed,exitCode,gws,coverage,channels});
        setOutputs(temp);
        setLoading(false);
    };

    return (
        outputs.length > 0 ?
            <div style={styles.container}>
                <h3>Results</h3>
                <Table>
                    <TableHead>
                        <TableRow>
                            <TableCell><b>#</b></TableCell>
                            <TableCell><b>Iterations</b></TableCell>
                            <TableCell><b>Elapsed</b></TableCell>
                            <TableCell><b>Exit Code</b></TableCell>
                            <TableCell><b>GWs</b></TableCell>
                            <TableCell><b>Coverage</b></TableCell>
                            <TableCell><b>Channels</b></TableCell>
                        </TableRow>
                    </TableHead>
                    <TableBody>
                        {outputs.map((output, index) => (
                            <TableRow key={index}>
                                <TableCell>{index+1}</TableCell>
                                <TableCell>{output.iters}</TableCell>
                                <TableCell>{output.elapsed} ms</TableCell>
                                <TableCell>{output.exitCode}</TableCell>
                                <TableCell>{output.gws}</TableCell>
                                <TableCell>{output.coverage*100} %</TableCell>
                                <TableCell>{output.channels}</TableCell>
                            </TableRow>
                        ))}
                    </TableBody>
                </Table>
            </div>
        : null
    )
};

export default OutputBlock;