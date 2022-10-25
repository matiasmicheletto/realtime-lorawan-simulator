import matplotlib as mpl
import seaborn as sns
import pandas as pd
import numpy as np

plt = mpl.pyplot
data = pd.read_csv("summary.csv")

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

# Poner en True para que se guarden las figuras
SAVEFIGS = False
mpl.rcParams['mathtext.fontset'] = 'stix'
mpl.rcParams['font.family'] = 'STIXGeneral'
mpl.rcParams['font.size'] = 12


# Grafico comparativo de tiempos de ejecucion random vs springs separando casos hard, medium y soft
fig, ax = plt.subplots(figsize=(10, 15))
ax.set_yscale("log") # Poner escala de tiempo en logaritmo
nonzero = data[data[ELAPSED] > 0]  # Eliminar registros con tiempo de ejecucion 0 para la escala logaritmica
sns.boxplot(x = PERIOD, y = ELAPSED, hue = ALGO, data = nonzero)
#plt.title(ALGO + ' vs ' + ELAPSED)
plt.xlabel(ALGO)
plt.ylabel(ELAPSED + ' [ms] (log scale)')
plt.grid()
if SAVEFIGS:
    plt.savefig('Box_Random_vs_New_Random.png')
plt.show()


fig, ax = plt.subplots(3,1, figsize=(10,15))
sns.lineplot(x = ED, y = ELAPSED, hue = ALGO, data = data[data[PERIOD]=="Soft"], ax=ax[0])
ax[0].grid()
ax[0].title.set_text("Soft")
sns.lineplot(x = ED, y = ELAPSED, hue = ALGO, data = data[data[PERIOD]=="Medium"], ax=ax[1])
ax[1].grid()
ax[1].title.set_text("Medium")
sns.lineplot(x = ED, y = ELAPSED, hue = ALGO, data = data[data[PERIOD]=="Hard"], ax=ax[2])
ax[2].grid()
ax[2].title.set_text("Hard")

if SAVEFIGS:
    plt.savefig('Line_Random_vs_New_Random.png')
plt.show()


