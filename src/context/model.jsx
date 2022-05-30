import React, {createContext} from 'react';
import Manager from 'lora-network-model/model-manager/index.mjs';

const model = new Manager();
model.initialize();

export const ModelCtx = createContext();

export const ModelProvider = props => (
    <ModelCtx.Provider value={model}>
        {props.children}
    </ModelCtx.Provider>
);