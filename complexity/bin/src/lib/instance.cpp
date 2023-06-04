#include "instance.h"

Instance::Instance(char* filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Failed to open input file." << endl;
        exit(1);
    }

    string line;
    while (getline(file, line)) {
        vector<int> row;
        stringstream ss(line);
        int number;
        while (ss >> number) {
            row.push_back(number);
            if (ss.peek() == ',')
                ss.ignore();
        }
        this->raw.push_back(row);
    }
    file.close();
}

Instance::~Instance() {

}

void Instance::printRawData() {
    for (const auto& row : this->raw) {
        for (int num : row) 
            cout << num << " ";
        cout << endl;
    }
}