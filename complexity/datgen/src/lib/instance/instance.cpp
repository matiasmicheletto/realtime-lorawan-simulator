#include "instance.h"

using namespace std;

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
        while (ss >> number)
            row.push_back(number);
        this->raw.push_back(row);
    }
    file.close();

    this->parseRawData();
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

void Instance::parseRawData() {
    // TODO: parse raw data
}
