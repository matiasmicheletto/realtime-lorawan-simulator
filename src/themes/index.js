import { createTheme } from '@mui/material/styles'; 

const theme = createTheme({
    typography: {
        fontFamily: 'Montserrat, Open Sans, sans-serif'
    },
    palette: {
        primary: {main: '#6E6E6E'},
        secondary: {main: '#F5F5F5'},
        text: {primary: '#000000',secondary: '#555555'}
    }
});

export default theme;