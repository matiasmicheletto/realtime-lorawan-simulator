
import matplotlib.pyplot as plt
import csv

def readfile(filename):
    with open(filename) as csvfile:
        r = csv.reader(csvfile, delimiter=',')
        data = list(r)
        return data

def plotdata(data, figname):
    fig = plt.figure()
    ax = fig.add_subplot(111)    
    ax.set_title(figname)    
    ax.set_xlabel('Network size (end devices)')
    ax.set_ylabel('Gateways')
    colors = {
        'Soft': 'g',
        'Medium': 'b',
        'Hard': 'r'
    }
    for i in range(len(data)):
        if i > 0:
            ax.scatter(int(data[i][4]), int(data[i][7]), c=colors[data[i][2]])    
    plt.grid(True)
    # save figure
    plt.savefig(figname)
    plt.show()

data_100x100 = readfile("summary_100x100.csv")
#data_1000x1000 = readfile("summary_1000x1000.csv")
#data_2000x2000 = readfile("summary_2000x2000.csv")

plotdata(data_100x100, "EDs_vs_GWs_100x100.png")
#plotdata(data_1000x1000, "EDs_vs_GWs_1000x1000.png")
#plotdata(data_2000x2000, "EDs_vs_GWs_2000x2000.png")
