import React from 'react';
import { Link, useLocation } from 'react-router-dom';
import { AppBar, Box, Button, Toolbar } from "@mui/material";

const Navigation = () => {

    const location = useLocation();
    const path = location.pathname.split('/')[1] || 'home';
    
    return (
        <Box sx={{ flexGrow: 1}} >
            <AppBar position="static" color="secondary">
                <Toolbar>
                    <Button LinkComponent={Link} to="/" color="inherit">
                        <span style={{fontWeight: path=="home" ? "bold" : "normal"}}>Home</span>
                    </Button>
                    <Button LinkComponent={Link} to="/config" color="inherit">
                        <span style={{fontWeight: path=="config" ? "bold" : "normal"}}>Configurator</span>
                    </Button>
                    <Button LinkComponent={Link} to="/solver" color="inherit">
                        <span style={{fontWeight: path=="solver" ? "bold" : "normal"}}>Solver</span>
                    </Button>
                    <Button LinkComponent={Link} to="/visualizer" color="inherit">
                        <span style={{fontWeight: path=="visualizer" ? "bold" : "normal"}}>Visualizer</span>
                    </Button>
                </Toolbar>
            </AppBar>
        </Box>
    );
};

export default Navigation;