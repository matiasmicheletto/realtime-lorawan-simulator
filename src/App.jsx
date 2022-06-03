import React from 'react';
import { BrowserRouter, Routes, Route } from 'react-router-dom';
import * as Views from './views';
import { ThemeProvider } from '@mui/material/styles'; 
import { ModelProvider, LoadingProvider } from './context';
import Navigation from './components/Navigation/';
import { CssBaseline } from '@mui/material';
import theme from './themes';

const App = () => (
    <ThemeProvider theme={theme}>
        <CssBaseline />
        <LoadingProvider />
            <ModelProvider>
                <BrowserRouter>
                    <Navigation/>
                    <Routes>
                        <Route index element={<Views.Home/>}/>                        
                    </Routes> 
                </BrowserRouter>
            </ModelProvider>
    </ThemeProvider>
);

export default App
