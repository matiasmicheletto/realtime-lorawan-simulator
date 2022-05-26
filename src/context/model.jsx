import React, {createContext} from 'react';
import LoRaWANModel from 'lora-network-model/network-model';

const model = new LoRaWANModel();

export const ModelCtx = createContext();

export const ModelProvider = props => (
    <ModelCtx.Provider value={model}>
        {props.children}
    </ModelCtx.Provider>
);