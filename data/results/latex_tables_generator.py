import pandas as pd

# File with data
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


def generateLatexTable(dataframe, mapsize, algo):
    # Encabezados
    header = ('\\begin{table}[htb]\n' # Tabla vertical (una sola pagina)
        '\t\\centering\n'
        '\t\\footnotesize\n'
        '\t\\begin{tabular}{|r|c|c|c|c|c|c|c|c|c|c|c|c|c|c|c|} \n'
        '\t\t\\hline\n'
        '\t\t\\multirow{2}{*}{ED} & \multicolumn{5}{c|}{Hard} & \multicolumn{5}{c|}{Medium} &\multicolumn{5}{c|}{Soft} \\\\ \n'
        '\t\t\\cline{2-16} \n'
        '\t\t&$SF_{m}$&GW & Ch & $\overline{U}$&T[s] &$SF_{m}$&GW & Ch & $\overline{U}$ &T[s] &$SF_{m}$&GW & Ch & $\overline{U}$  & T[s]  \\\\ \n'
        '\t\t\\hline \n')

    text = header

    averagesSoft = dataframe[(dataframe[MAP] == mapsize) & (dataframe[PERIOD] == "Soft") & (dataframe[ALGO] == algo)].groupby(ED).mean()
    averagesMedium = dataframe[(dataframe[MAP] == mapsize) & (dataframe[PERIOD] == "Medium") & (dataframe[ALGO] == algo)].groupby(ED).mean()
    averagesHard = dataframe[(dataframe[MAP] == mapsize) & (dataframe[PERIOD] == "Hard") & (dataframe[ALGO] == algo)].groupby(ED).mean()
    
    tableSoft = averagesSoft[[SF, GW, CH, GWUF, ELAPSED]]
    tableMedium = averagesMedium[[SF, GW, CH, GWUF, ELAPSED]]
    tableHard = averagesHard[[SF, GW, CH, GWUF, ELAPSED]]

    # Combine tables
    table = pd.concat([tableHard, tableMedium, tableSoft], axis=1)

    # Rellenar los campos de cada item
    for row in table.iterrows():        
        line = " & ".join([str(round(r,2)) for r in [
            row[0], 
            row[1][SF][0], 
            row[1][GW][0], 
            row[1][CH][0], 
            row[1][GWUF][0], 
            row[1][ELAPSED][0]/1000,
            row[1][SF][1], 
            row[1][GW][1], 
            row[1][CH][1], 
            row[1][GWUF][1], 
            row[1][ELAPSED][1]/1000,
            row[1][SF][2], 
            row[1][GW][2], 
            row[1][CH][2], 
            row[1][GWUF][2], 
            row[1][ELAPSED][2]/1000
        ]])
        text = text + '\t\t' + line + '\\\\ \n'

    # Cierre de la tabla
    footer = ('\t\t\\hline \n'
        '\t\\end{tabular} \n'
        '\t\\caption{Uniform distribution, '+algo+', '+str(mapsize*mapsize)+' $m^2$} \n'
        '\t\\label{tab:Un'+algo+''+str(mapsize)+'} \n'
        '\\end{table} \n')

    text = text + footer
    
    with open("Uniform_"+algo+"_"+str(mapsize)+".tex", 'w') as output_file:
        output_file.write(text)


generateLatexTable(data, 100, "Random")
generateLatexTable(data, 1000, "Random")
generateLatexTable(data, 2000, "Random")

generateLatexTable(data, 100, "Springs")
generateLatexTable(data, 1000, "Springs")
generateLatexTable(data, 2000, "Springs")

