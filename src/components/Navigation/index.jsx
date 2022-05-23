import React from 'react';
import { Link, useLocation } from 'react-router-dom';
import { AppBar, Box, Button, Toolbar } from "@mui/material";

const Navigation = () => {

    const location = useLocation();
    const path = location.pathname.split('/')[1] || 'home';
    
    return (
        <Box sx={{ flexGrow: 1}} >
            <AppBar position="static">
                <Toolbar>
                    <Button LinkComponent={Link} to="/" color="inherit">
                        <span style={{fontWeight: path=="home" ? "bold" : "normal"}}>Home</span>
                    </Button>
                    <Button LinkComponent={Link} to="/config" color="inherit">
                        <span style={{fontWeight: path=="config" ? "bold" : "normal"}}>Config</span>
                    </Button>
                </Toolbar>
            </AppBar>
        </Box>
    );
};

export default Navigation;