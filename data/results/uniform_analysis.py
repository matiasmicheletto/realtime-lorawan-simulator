import matplotlib as mpl
import seaborn as sns
import pandas as pd
import numpy as np

plt = mpl.pyplot

# File with data to analyze
data = pd.read_csv("summary_uniform.csv")
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


"""
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
    plt.savefig('Springs_vs_Random.png')
plt.show()
"""




"""
def getEDvsGWAvg(alg, per, mapsize):
    if alg == "Random" and per == "Hard" and mapsize == 2000: # Limitado para menos de 5.000 EDs
        sample = data[(data[ALGO] == alg) & (data[PERIOD] == per) & (data[MAP] == mapsize) & (data[ED] <= 5000)]
        return sample.groupby(ED).mean()[GW]
    sample = data[(data[ALGO] == alg) & (data[PERIOD] == per) & (data[MAP] == mapsize)]
    return sample.groupby(ED).mean()[GW]


#plt.figure(figsize=(10, 20))
#for a, algo in enumerate(["Random", "Springs"]):    
algo = "Random"
for m, mapsize in enumerate([100, 1000, 2000]):
    #plt.subplot(3, 2, 2*m+1 + a)
    #plt.subplot(3, 1, m+1)
    plt.figure(figsize=(10, 5))
    plt.plot(getEDvsGWAvg(algo, 'Soft', mapsize), '-o', label = 'Soft')
    plt.plot(getEDvsGWAvg(algo, 'Medium', mapsize), '-x', label = 'Medium')
    plt.plot(getEDvsGWAvg(algo, 'Hard', mapsize), '-s', label = 'Hard')
    plt.xlabel('ED Number')
    plt.ylabel('GW Number')
    #plt.title(algo + ' - Map size = '+str(mapsize)+'x'+str(mapsize))
    plt.legend()
    plt.grid()
    if SAVEFIGS:
        plt.savefig('ED vs GW '+algo + '_' + str(mapsize) + '.png')
    plt.show()

# Para subplots en columna
#top=0.94
#bottom=0.07
#left=0.125
#right=0.9
#hspace=0.3
#wspace=0.2
#plt.subplots_adjust(top=top, bottom=bottom, left=left, right=right, hspace=hspace, wspace=wspace)
#if SAVEFIGS:
#    plt.savefig('ED vs GW.png')
#plt.show()
"""

