import { createTheme } from '@mui/material/styles'; 

const theme = createTheme({
    typography: {
        fontFamily: 'Montserrat, Open Sans, sans-serif'
    },
    palette: {
        primary: {main: '#0B7F14'},
        secondary: {main: '#834702'},
        text: {primary: '#000000',secondary: '#555555'}
    }
});

export default theme;