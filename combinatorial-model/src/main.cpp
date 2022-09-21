#include <iostream>
#include <vector>
#include <string.h>
#include <stdio.h>

#define LINE_BUFFER_SIZE 256
#define FILE_COLS 4
#define INPUT_FILE_NAME "input.csv"

inline int gcd(int a, int b) {return b == 0 ? a : gcd(b, a % b);}
inline int lcm(int a, int b){return a * b / gcd(a, b);}

using namespace std;

struct node {
    unsigned int id;
    double x;
    double y;
    unsigned int period;
};


vector<node>* loadInstance(const char* filename, unsigned int &H) {
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file csv file\n");        
        exit(1);
    }

    H = 1;
    vector<node> *network = new vector<node>;
    char line[LINE_BUFFER_SIZE];
    while ( fgets(line, LINE_BUFFER_SIZE, file) != NULL ) {
        int col = 0;
        char* token;
        unsigned int id, period;
        double x, y;        
        for (token = strtok(line, ","); token != NULL && col < FILE_COLS; col++, token=strtok(NULL, ",")){
            switch (col){
                case 0: 
                    id = atoi(token);
                    break;
                case 1:
                    x = atof(token);
                    break;
                case 2:
                    y = atof(token);
                    break;
                case 3:
                    period = atoi(token);
                    H = (unsigned int) lcm(H, period);
                    break;
                default:
                    break;
            }
        }
        network->push_back({id, x, y, period});
    }
    fclose(file);
    
    printf("-----------------------------------\n");
    printf("Configuration file loaded\n");
    printf("End devices: %ld\n", network->size());
    printf("System hyperperiod: %d\n", H);        
    printf("-----------------------------------\n");

    return network;
}

int main(int argc, char **argv) {
    unsigned int H;
    vector<node> *network = loadInstance(INPUT_FILE_NAME, H);
    delete network;
    return 0;
}

