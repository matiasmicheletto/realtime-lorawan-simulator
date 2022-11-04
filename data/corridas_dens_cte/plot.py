import matplotlib as mpl
import seaborn as sns
import pandas as pd
import numpy as np
plt = mpl.pyplot

data = pd.read_csv("summary.csv")
SAVEFIGS = False

# Column names
ALGO = 'GW Pos. Heuristic'
SCHED = 'ED Sched. Method'
POS = 'Position distr.'
PERIOD = 'Periods distr.'
MAP = 'Map size'
ED = 'ED Number'
DENS = 'ED Dens. (1/m^2)'
SF = 'Max. SF'
GW = 'GW Number'
CH = 'Channels used'
UNF = 'ED Unfeas. Instances'
COV = 'Coverage %'
GWUF = 'GW Avg. UF'
ELAPSED = 'Elapsed'
ITERS = 'Iterations'
EXIT = 'Exit condition'

mpl.rcParams['mathtext.fontset'] = 'stix'
mpl.rcParams['font.family'] = 'STIXGeneral'
mpl.rcParams['font.size'] = 12


fig, ax = plt.subplots(figsize=(10, 15))
#ax.set_yscale("log") 
#nonzero = data[data[ELAPSED] > 0] 
sns.scatterplot(x = MAP, y = GW, hue = ALGO, data = data)
#plt.title(ALGO + ' vs ' + ELAPSED)
plt.xlabel(MAP)
plt.ylabel(GW)
plt.grid()
if SAVEFIGS:
    plt.savefig('GWvsMAP.png')
plt.show()
