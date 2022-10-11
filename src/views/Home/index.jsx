import React from 'react';
import MainView from '../../templates/MainView';
import ConfigForm from '../../components/ConfigForm';
import NetworkViewer from '../../components/NetworkViewer';
import OutputBlock from '../../components/OutputBlock';

const Home = () => (
    <MainView>
        <ConfigForm />
        <OutputBlock />
        <NetworkViewer />
    </MainView>
);

export default Home;