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

