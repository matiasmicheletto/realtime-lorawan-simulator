import React from 'react';
import MainView from '../../templates/MainView';
import ConfigForm from '../../components/ConfigForm';

const Home = () => {
    return (
        <MainView>
            <h3>Model configuration</h3>
            <ConfigForm />
        </MainView>
    );
};

export default Home;