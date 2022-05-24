import React, {createContext} from 'react';
import LoRaWANModel from 'lora-network-model/index.js';

const model = new LoRaWANModel();

export const ModelCtx = createContext();

export const ModelProvider = props => (
    <ModelCtx.Provider value={model}>
        {props.children}
    </ModelCtx.Provider>
);