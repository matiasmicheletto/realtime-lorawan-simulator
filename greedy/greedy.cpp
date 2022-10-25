#include <iostream>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <algorithm>
#include <random>

using namespace std;


unsigned int _enddevices = 10000;
unsigned int _mapsize = 1000;
      
vector<vector<int>> ADY;
vector<vector<float>> nodos;


void genera(){    
    int mcm = 3600;    
    int periodo;
    vector<float> ed;

    for (long i = 0; i < _enddevices; i++) {
        // Vector to store column elements
        ed.push_back(i);
        float posx = _mapsize * (rand() / (RAND_MAX + 1.0));
        float posy = _mapsize * (rand() / (RAND_MAX + 1.0));
        ed.push_back(posx);
        ed.push_back(posy);
        float distancia = sqrt(posx*posx + posy*posy);
        ed.push_back(distancia);
        
        int per = 1000 * (rand() / (RAND_MAX + 1.0));
        if (per <= 250) {
            periodo = 400;
        }else if (per>250 && per <= 500){
            periodo = 800;
        }else if (per>500 && per <= 750){
            periodo = 1600;
        }else if (per>750 && per <= 1000){
            periodo = 3200;
        }    
        ed.push_back(periodo);
        ed.push_back(0);
        ed.push_back(0);
        ed.push_back(mcm/periodo); 
        ed.push_back(0);                   
        nodos.push_back(ed);
        ed.clear();
    }    
}

void adyacencia(){    
    float dists[6] = {2000, 1000, 500, 250, 125, 62.5};
    for (unsigned int i = 0; i< nodos.size(); i++){
        for (unsigned int j = i; j< nodos.size(); j++){    
            float dx = nodos[i][1]-nodos[j][1]; 
            float dy = nodos[i][2]-nodos[j][2];
            float euc = sqrt(dx*dx + dy*dy);
            for(unsigned int k = 0; k < 6; k++){    
                if (euc < dists[k]) {
                    ADY[i][j]=12-k;
                    ADY[j][i]=12-k;
                }    
            }
        }
    }
}


void greedy(){    
    int final=0;
    int h;
    vector<int> init,cero;
    vector<vector<long int>> asignados;
    vector<long int> parcial, GWS;
    
    int i,j;
    vector<int>::iterator it;
    long Best[_enddevices];
    long best, Max;
    srand(time(NULL) ^ (getpid()<<16));
    
    float **Usf = (float**) malloc(sizeof(float*)*6);
    for(int sf = 0; sf < 6; sf++){
        Usf[sf] = (float*) malloc(sizeof(float)*_enddevices);
        for(int k = 0; k < _enddevices; k++){
            Usf[sf][k] = 0;
        }
    }
    
    cout<<"Genera sistema"<<endl;
    genera();
    
    cout<<"Calcula adyacencia"<<endl;
    for (i=0;i<nodos.size();i++)
        init.push_back(0);
    for (i=0;i<nodos.size();i++)
        ADY.push_back(init);
    cero = init;
    adyacencia();
    
    //Para seleccionar el mejor nodo.
    cout<<"Inicia Asignación"<<endl;
    bool sigo = true;
    int gw = 0;
    while (sigo) {
        gw++;    
        //cout<<"Paso "<<gw<<endl;
        
        best=0;
        Max=0;

        for(i=0;i<ADY.size();i++) {
            Best[i]=0;
            Best[i]=ADY[i].size() - count(ADY[i].begin(), ADY[i].end(), 0);
            if (Best[i]>Max){
                best=i;
                Max=Best[i];
            }
        }

        GWS.push_back(best);
        for(int k = 0; k < 6; k++){
            Usf[k][best]=0;
        }
        
        for(j=0;j<ADY.size();j++){
            for(int k = 0; k < 6; k++){
                if (ADY[best][j] == k+7){
                    float k2 = pow(2, k);
                    float required = k2 / (nodos[j][4]-k2);
                    if (Usf[k][best] + required < 1){
                        Usf[k][best] += required;                        
                        parcial.push_back(j);
                        for(long h=0; h<ADY.size(); h++)
                            ADY[h][j]=0;
                    }
                }    
            }
        }
        for (long h = 0; h < parcial.size(); h++)
            ADY[parcial[h]] = cero;
        
        asignados.push_back(parcial);
        parcial.clear();
        sigo = false;
        for(i=0; i<ADY.size(); i++){
            if(ADY[i].size()-count(ADY[i].begin(), ADY[i].end(), 0) > 0){
                sigo = true;
                break;
            }
        }
    }

    parcial.clear();
    for (i=0;i<asignados.size();i++){
        parcial = asignados[i];
        
        /*
        for(j=0;j<parcial.size();j++){
            cout<<parcial[j]<<",";
        }
        cout<<endl;
        */

        parcial.clear();
    }
    
    
    cout<<"Gateways Necesarios " << gw << endl;
    /*
    cout<<"Lista de gw:"<<endl;
    for(long h=0; h<GWS.size(); h++)
        cout << GWS[h] << ",";
    cout<<endl;        
    */
}

int main(){
    greedy();
    return 0;
}
