import matplotlib as mpl
import seaborn as sns
import pandas as pd
import numpy as np
plt = mpl.pyplot

data = pd.read_csv("paretolog.csv", names=['Algo', 'GW', 'NCED', 'IsOpt'], header=None)
SAVEFIGS = True

mpl.rcParams['mathtext.fontset'] = 'stix'
mpl.rcParams['font.family'] = 'STIXGeneral'
mpl.rcParams['font.size'] = 12

data.replace({  
    'Algo': {  
        0: 'GA',  
        1: 'Random',
        2: 'Greedy',
        3: 'Springs' 
    }  
}, inplace = True)  

fig, ax = plt.subplots(figsize=(10, 10))
sns.scatterplot(x = 'NCED', y = 'GW', hue = 'Algo', data = data[data['IsOpt'] == 0])
plt.xlabel('NC ED')
plt.ylabel('GW')
plt.title('Objective space')
plt.grid()
if SAVEFIGS:
    plt.savefig('Figura.png')
plt.show()
