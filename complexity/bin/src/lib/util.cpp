#include "util.h"

void arrayCopy(unsigned int **from, unsigned int **to, int xDim, int yDim){
    for(int i = 0; i < xDim; i++)
        for(int j = 0; j < yDim; j++)
            to[i][j] = from[i][j];
}