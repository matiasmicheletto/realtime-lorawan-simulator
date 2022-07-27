import matplotlib.pyplot as plt
import csv

#Indexes
ALGO = 0
SCHED = 1
POS = 2
PERIOD = 3
MAP = 4
ED = 5
DENS = 6
SF = 7
GW = 8
CH = 9
UNF = 10
COV = 11
GWUF = 12
ELAPSED = 13
ITERS = 14
EXIT = 15

def readfile(filename):
    with open(filename) as csvfile:
        r = csv.reader(csvfile, delimiter=',')
        data = list(r)
        return data


# Plot ed vs gw
def plotEDvsGW(data, figname):
    fig, ax = fig.subplots(3, 1)    
    
    ax.set_title(figname)    
    ax.set_xlabel('Network size (end devices)')
    ax.set_ylabel('Gateways')
    
    colors = {
        'Soft': 'g',
        'Medium': 'b',
        'Hard': 'r'
    }
    
    rows_100 = [r for r in data if r[MAP] == 100]
    rows_1000 = [r for r in data if r[MAP] == 1000]
    rows_2000 = [r for r in data if r[MAP] == 2000]

    for row in rows_100:
        if i > 0:
            ax[0].scatter(int(row[ED]), int(row[GW]), c=colors[row[PERIOD]])    

    for row in rows_1000:
        if i > 0:
            ax[1].scatter(int(row[ED]), int(row[GW]), c=colors[row[PERIOD]])    

    for row in rows_2000:
        if i > 0:
            ax[2].scatter(int(row[ED]), int(row[GW]), c=colors[row[PERIOD]])    

    plt.grid(True)
    
    # save figure
    if(figname != None):
        plt.savefig(figname)

    plt.show()


# Plot ed vs elapsed time
def plotEDvsElapsed(data, figname):
    fig, ax = fig.subplots(3, 1)    
    
    ax.set_title(figname)    
    ax.set_xlabel('Network size (end devices)')
    ax.set_ylabel('Elapsed time (s)')
    
    colors = {
        'Soft': 'g',
        'Medium': 'b',
        'Hard': 'r'
    }
    
    rows_100 = [r for r in data if r[MAP] == 100]
    rows_1000 = [r for r in data if r[MAP] == 1000]
    rows_2000 = [r for r in data if r[MAP] == 2000]

    for row in rows_100:
        if i > 0:
            ax[0].scatter(int(row[ED]), float(row[ELAPSED]), c=colors[row[PERIOD]])    

    for row in rows_1000:
        if i > 0:
            ax[1].scatter(int(row[ED]), float(row[ELAPSED]), c=colors[row[PERIOD]])    

    for row in rows_2000:
        if i > 0:
            ax[2].scatter(int(row[ED]), float(row[ELAPSED]), c=colors[row[PERIOD]])    

    plt.grid(True)
    
    # save figure
    if(figname != None):
        plt.savefig(figname)

    plt.show()


data = readfile("summary_uniform.csv")
plotEDvsGW(data, "uniform_ed_gw.png")
plotEDvsElapsed(data, "uniform_ed_elapsed.png")
