import React from 'react';
import { CircularProgress } from '@mui/material';
import classes from './style.module.css';

const Preloader = () => ( // Full screen preloader
    <div className={classes.Container}>
        <div className={classes.SpinnerContainer}>
            <CircularProgress />
        </div>
    </div>
);

export default Preloader;