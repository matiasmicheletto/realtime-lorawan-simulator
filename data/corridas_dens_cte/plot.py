import matplotlib as mpl
import seaborn as sns
import pandas as pd
import numpy as np
plt = mpl.pyplot

data = pd.read_csv("summary.csv")
SAVEFIGS = True

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


fig, ax = plt.subplots(2, 1, figsize=(10, 15))
sns.lineplot(x = MAP, y = GW, hue = ALGO, data = data, ax=ax[0])
sns.lineplot(x = MAP, y = ELAPSED, hue = ALGO, data = data, ax=ax[1])
ax[0].title.set_text(MAP + ' vs ' + GW)
ax[1].title.set_text(MAP + ' vs ' + ELAPSED)
ax[0].set_ylabel(GW)
ax[1].set_ylabel(ELAPSED)
plt.xlabel(MAP)
ax[0].grid()
ax[1].grid()
if SAVEFIGS:
    plt.savefig('Figura.png')
plt.show()
