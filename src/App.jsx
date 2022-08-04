import React from 'react';
import { BrowserRouter, Routes, Route } from 'react-router-dom';
import * as Views from './views';
import { ThemeProvider } from '@mui/material/styles'; 
import LoadingProvider from './context/loading';
import Navigation from './components/Navigation/';
import { CssBaseline } from '@mui/material';
import theme from './themes';


const App = () => (
    <ThemeProvider theme={theme}>
        <CssBaseline />
            <LoadingProvider >
                <BrowserRouter>
                    <Navigation/>
                    <Routes>
                        <Route index element={<Views.Home/>}/>
                        <Route path='/help/' element={<Views.Help/>}/>
                        <Route path='/about/' element={<Views.About/>}/>
                    </Routes> 
                </BrowserRouter>
            </LoadingProvider>
    </ThemeProvider>
);

export default App
