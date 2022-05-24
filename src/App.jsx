import React from 'react';
import { BrowserRouter, Routes, Route } from 'react-router-dom';
import * as Views from './views';
import { ThemeProvider } from '@mui/material/styles'; 
import { ModelProvider } from './context';
import Navigation from './components/Navigation/';
import { CssBaseline } from '@mui/material';
import theme from './themes';

const App = () => (
    <ThemeProvider theme={theme}>
        <CssBaseline />
            <ModelProvider>
                <BrowserRouter>
                    <Navigation/>
                    <Routes>
                        <Route index element={<Views.Home/>}/>
                        <Route path='/config' element={<Views.Config/>}/>
                        {/*<Route path='/config' element={<Views.Config/>}/>*/}
                        <Route path='/visualizer' element={<Views.Visualizer/>}/>
                    </Routes> 
                </BrowserRouter>
            </ModelProvider>
    </ThemeProvider>
);

export default App
