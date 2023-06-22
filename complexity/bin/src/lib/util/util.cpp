#include "util.h"

void printHelp() {    
    std::ifstream manualFile("readme.txt");
    if (manualFile.is_open()) {
        std::string line;
        while (getline(manualFile, line)) {
            std::cout << line << std::endl;
        }
        manualFile.close();
        exit(1);
    } else {
        std::cerr << "Error: Unable to open manual file." << std::endl;
    }
}

void arrayCopy(unsigned int **from, unsigned int **to, int xDim, int yDim){
    for(int i = 0; i < xDim; i++)
        for(int j = 0; j < yDim; j++)
            to[i][j] = from[i][j];
}