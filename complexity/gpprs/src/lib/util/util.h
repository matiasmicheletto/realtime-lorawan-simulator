#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <fstream>

void printHelp();
void arrayCopy(unsigned int **from, unsigned int **to, int xDim, int yDim);
unsigned int sf2e(unsigned int sf);
//void append2log(unsigned int gwCount, unsigned int energy, double totalUF, double objective, int64_t duration, unsigned long iters)

#endif // UTIL_H