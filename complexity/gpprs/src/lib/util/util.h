#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <vector>
#include <fstream>


void printHelp();
void arrayCopy(unsigned int **from, unsigned int **to, int xDim, int yDim);
unsigned int sf2e(unsigned int sf);
template <typename T>
void removeRowsAndColumn(std::vector<std::vector<T>>& matrix, size_t columnIndex, const std::vector<size_t>& rowIndices);
template <typename T>
std::vector<std::vector<T>> copyMatrix(const std::vector<std::vector<T>>& matrix);

#endif // UTIL_H