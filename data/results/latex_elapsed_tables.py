import pandas as pd

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

filename = "random_vs_springs_elapsed.tex"
data = pd.read_csv("summary_all.csv")
data = data[data[EXIT] != "Timeout"]
table_data = data.groupby([POS, PERIOD, MAP, ALGO]).mean()[ELAPSED].unstack(level=0).unstack(level=0)
print(table_data)

header = ('\\begin{table}[htb]\n' 
        '\t\\centering\n'        
        '\t\\begin{tabular}{|r|r|c|c|c|c|c|c|} \n'
        '\t\t\\hline\n'
        '\t\t\\multirow{2}{*}{Map size} & \\multirow{2}{*}{Pos. method} & \multicolumn{3}{c|}{Clouds} & \multicolumn{3}{c|}{Uniform} \\\\ \n'
        '\t\t\\cline{3-8} \n'
        '\t\t & & Hard & Medium & Soft & Hard & Medium & Soft\\\\ \n'
        '\t\t\\hline \n')
footer = ('\t\t\\hline \n'
        '\t\\end{tabular} \n'
        '\t\\caption{Comparison of optimization total elapsed time when using Random strategy vs. pseudo-springs} \n'
        '\t\\label{tab:timeper} \n'
        '\\end{table} \n')
content = header
for row in table_data.iterrows():
    content = content + '\t\t' + str(row[0][0]) + " & " + str(row[0][1]) + " & "
    content = content + " & ".join([str(round(row[1][r]/1000, 2)) for r in range(0, 6)])
    content = content + " \\\\ \n"
content = content + footer

with open(filename, 'w') as latexfile:
    latexfile.write(content)
