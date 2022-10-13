import React from 'react';
import MainView from '../../templates/MainView';

const Help = () => {
    return (
        <MainView>
            <h3>How to use this software</h3>
            <ul></ul>
            <li>The first pannel, allows to set the instance configuration. The default configuration may take less than a minute to solve, but take into consideration that this time may vary depending on the parameters configured.</li>
            <li>Hit the "RUN INSTANCE" button to execute the optimization algorithm. In case it takes to long, you can view the developer console for error messages. In Chrome or Firefox, the console shows up pressing the F12 key. If the app stops responding, hit F5 to reload and modify the optimization parameters.</li>
            <li>When the optimization ends, a "Results" block will appear with a table containing the optimization results.</li>
            <li>To visualize the network evolution during the optimization process, press the "RUN ANIMATION" button.</li>
            <li>In case of more help needed, you can see the repository <a href="https://github.com/matiasmicheletto/realtime-lorawan-simulator" target="_blank">page</a> with the source code of this web application and open an issue, if necessary.</li>
        </MainView>
    );
};

export default Help;