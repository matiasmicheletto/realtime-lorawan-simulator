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

template <typename T>
void removeRowsAndColumn(std::vector<std::vector<T>>& matrix, size_t columnIndex, const std::vector<size_t>& rowIndices) {
    for (auto it = rowIndices.rbegin(); it != rowIndices.rend(); ++it)
        if (*it < matrix.size())
            matrix.erase(matrix.begin() + *it);

    for (auto& row : matrix)
        if (columnIndex < row.size())
            row.erase(row.begin() + columnIndex);
}

template <typename T>
std::vector<std::vector<T>> copyMatrix(const std::vector<std::vector<T>>& matrix) {
    std::vector<std::vector<T>> copy;
    copy.reserve(matrix.size());

    for (const auto& row : matrix)
        copy.push_back(row);

    return copy;
}

unsigned int sf2e(unsigned int sf){
    static const unsigned int arr[6] = {1, 2, 4, 8, 16, 32};
    return arr[sf-7];
}