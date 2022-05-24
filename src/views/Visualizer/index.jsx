import React from 'react';
import MainView from '../../templates/MainView';
import NetworkViewer from '../../components/NetworkViewer';

const Visualizer = () => {
    return (
        <MainView>
            <h3>Current network</h3>
            <NetworkViewer />
        </MainView>
    );
};

export default Visualizer;