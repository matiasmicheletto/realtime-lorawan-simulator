import React from 'react';
import MainView from '../../templates/MainView';
import Network from '../../components/Network';

const Home = () => {
    return (
        <MainView>
            <h3>Current network</h3>
            <Network />
        </MainView>
    );
};

export default Home;