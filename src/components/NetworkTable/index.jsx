import React, {useState, useContext} from 'react';
import { ModelCtx } from '../../context';
import Paper from '@mui/material/Paper';
import Table from '@mui/material/Table';
import TableBody from '@mui/material/TableBody';
import TableCell from '@mui/material/TableCell';
import TableContainer from '@mui/material/TableContainer';
import TableHead from '@mui/material/TableHead';
import TablePagination from '@mui/material/TablePagination';
import TableRow from '@mui/material/TableRow';

const columns = [
    { id: 'id', label: 'ID', minWidth: 150 },
    { id: 'label', label: 'Name', minWidth: 150 },
    { id: 'group', label: 'Group', minWidth: 150 },
    { id: 'x', label: 'Lat.', minWidth: 150, format: value => value.toFixed(2) },
    { id: 'y', label: 'Long.', minWidth: 150, format: value => value.toFixed(2) },
    { id: 'period', label: 'Period', minWidth: 150 }
];

const NetworkTable = () => {

    const model = useContext(ModelCtx);
    const rows = model.getNetwork();

    const [page, setPage] = useState(0);
    const [rowsPerPage, setRowsPerPage] = useState(10);

    const handleChangePage = (event, newPage) => {
        setPage(newPage);
    };

    const handleChangeRowsPerPage = event => {
        setRowsPerPage(+event.target.value);
        setPage(0);
    };

    return (
        <Paper sx={{ width: '100%', overflow: 'hidden', marginTop: '10px', boxShadow: '3px 3px 10px gray' }}>
            <TableContainer sx={{ maxHeight: 440 }}>
                <Table stickyHeader aria-label="sticky table">
                    <TableHead>
                        <TableRow>
                            {
                                columns.map((column) => (
                                    <TableCell
                                        key={column.id}
                                        align={column.align}
                                        style={{ minWidth: column.minWidth }}>
                                        {column.label}
                                        </TableCell>
                                    ))}
                        </TableRow>
                    </TableHead>
                    <TableBody>
                        {
                            rows.slice(page*rowsPerPage, page*rowsPerPage+rowsPerPage).map(row => (
                                <TableRow hover role="checkbox" tabIndex={-1} key={row.id} style={{height: 20}}>
                                {columns.map((column) => {
                                    const value = row[column.id];
                                    return (
                                        <TableCell key={column.id} align={column.align}>
                                            {column.format && typeof value === 'number' ? column.format(value) : value}
                                        </TableCell>
                                    );
                                })}
                                </TableRow>
                            ))
                        }
                    </TableBody>
                </Table>
            </TableContainer>
            <TablePagination
                rowsPerPageOptions={[10, 25, 100]}
                component="div"
                count={rows.length}
                rowsPerPage={rowsPerPage}
                page={page}
                onPageChange={handleChangePage}
                onRowsPerPageChange={handleChangeRowsPerPage}/>
        </Paper>
    );
}

export default NetworkTable;