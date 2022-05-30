import React from 'react';
import MainView from '../../templates/MainView';
import ConfigForm from '../../components/ConfigForm';
import NetworkViewer from '../../components/NetworkViewer';
import NetworkTable from '../../components/NetworkTable';

const Home = () => {
    return (
        <MainView>
            <h3>Model configuration</h3>
            <ConfigForm />
            <h3>Network topology</h3>
            <NetworkViewer />
            <h3>List of network nodes</h3>
            <NetworkTable />
        </MainView>
    );
};

export default Home;