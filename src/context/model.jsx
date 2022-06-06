import React, { useState, createContext, useEffect } from 'react';
import Manager from 'lora-network-model/model-manager/index.mjs';

export const manager = new Manager();
manager.initialize();

export const ModelContext = createContext();

export const ModelProvider = props => {

    const [model, setModel] = useState(manager.getResults());

    useEffect( () => {
        manager.onChange = setModel;
    }, []);

    return (
        <ModelContext.Provider value={{model, setModel}}>
            {props.children}
        </ModelContext.Provider>
    );
};