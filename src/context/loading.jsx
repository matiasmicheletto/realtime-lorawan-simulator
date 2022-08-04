import React, {useState, createContext} from 'react';
import Preloader from '../components/Preloader';

export const LoadingContext = createContext({
    loading: false,
    setLoading: ()=>{}
});

const LoadingProvider = ({ children }) => {
	const [loading, setLoading] = useState(false);

	return (
		<LoadingContext.Provider value={{loading, setLoading}}>
			{loading && <Preloader />}
			{children}
		</LoadingContext.Provider>
	);
};

export default LoadingProvider;