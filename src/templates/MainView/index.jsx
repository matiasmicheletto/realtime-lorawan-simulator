import React from 'react';
import { Container } from '@mui/material';
import classes from './style.module.css';

const MainView = props => (    
    <Container className={classes.Container}>
        {props.children}
    </Container>
);

export default MainView;