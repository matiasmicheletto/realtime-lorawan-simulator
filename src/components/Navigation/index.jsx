import React from 'react';
import { Link, useLocation } from 'react-router-dom';
import { AppBar, Box, Button, Toolbar } from "@mui/material";

const Navigation = () => {

    const location = useLocation();
    const path = location.pathname.split('/')[1] || 'home';
    
    return (
        <Box sx={{ flexGrow: 1}} >
            <AppBar position="static" color="primary">
                <Toolbar>
                    <Button LinkComponent={Link} to="/" color="inherit">
                        <span style={{fontWeight: path=="home" ? "bold" : "normal"}}>Home</span>
                    </Button>
                    <Button LinkComponent={Link} to="/help/" color="inherit">
                        <span style={{fontWeight: path=="help" ? "bold" : "normal"}}>Help</span>
                    </Button>
                    <Button LinkComponent={Link} to="/about/" color="inherit">
                        <span style={{fontWeight: path=="about" ? "bold" : "normal"}}>About</span>
                    </Button>
                </Toolbar>
            </AppBar>
        </Box>
    );
};

export default Navigation;