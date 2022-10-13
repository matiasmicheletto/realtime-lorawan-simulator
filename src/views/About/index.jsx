import React from 'react';
import MainView from '../../templates/MainView';

const About = () => {
    return (
        <MainView>
            <h3>About this software</h3>
            <p>This web application consists on a tool for generate, analyze and visualize city-wide, real-time LoRaWAN network model composed of a few thousand to millions end devices.</p>
            <p>Using this online tool, the user can test different algorithms to determine the minimum number of gateways and their positions in order to connect the entire list of end devices.</p>
            <p>Each end device has to transmit a single periodic message to a gateway from a fixed position and the locations and periods of all end devices in the network can be loaded from an external file or be randomly generated. The goal is to determine where to locate the gateways in order to achieve a fully connected network. The conditions to be fulfilled for an end device to be connected to a gateway are the following:</p>
            <p>The gateways and end devices can be connected through one of 16 different and orthogonal channels.</p>
            <p>The gateways and end devices can be connected using one of six different and orthogonal spreading factors.</p>
            <p>Each spreading factor has its maximum range and minimum transmission period, wich are listed below:</p>
            <ul>
                <li>SF7: max range is 62.5m and min period is 100 slots.</li>
                <li>SF8: max range is 125m and min period is 200 slots.</li>
                <li>SF9: max range is 250m and min period is 400 slots.</li>
                <li>SF10: max range is 500m and min period is 800 slots.</li>
                <li>SF11: max range is 1000m and min period is 1600 slots.</li>
                <li>SF12: max range is 2000m and min period is 3200 slots.</li>
            </ul>
            <footer>
                <h3>Authors</h3>
                <b>System model:</b> Dr. Rodrigo Santos.
                <br/>
                <b>Implementation:</b> Dr. Matías Micheletto.
                <br/>
                <a href="https://www.diec.uns.edu.ar/rts/#/" target="_blank">Real Time and Embedded Systems | ICIC UNS-CONICET</a> 
                <br/>
                <a href="https://www.diec.uns.edu.ar/" target="_blank">Dept. of Electrical Engineering and Computers | National University of the South.</a>
                <br />
                <a href="https://github.com/matiasmicheletto/realtime-lorawan-simulator" target="_blank">Source code</a>
            </footer>
        </MainView>
    );
};

export default About;