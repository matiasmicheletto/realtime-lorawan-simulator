#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sys/stat.h>
#include <sys/errno.h>
#include <dirent.h>
#include <algorithm>
#include "lib/instancia.h"
#include "lib/util.h"
#include "lib/ed.h"
#include "lib/heu.h"
//#include "lib/ilp.h"
#include <boost/date_time.hpp>
//#include <math>
#define D7 62.5
#define D8 125
#define D9 250
#define D10 500
#define D11 1000
#define D12 2000  
  vector<float> ed;
  vector<float> gw;
  vector<vector<float>> nodosOR,nodos,devices,Fnodes,NFnodes,Msgs7,Msgs8,Msgs9,Msgs10,Msgs11,Msgs12,Msgs7F,Msgs8F,Msgs9F,Msgs10F,Msgs11F,Msgs12F,GWS;
  int canal = mirand(100);
  int eds=10;
  int lado=1000;
  float distancia;
  int mcm=3600;
  int periodo;
  int posx,posy,per;
  long MsgToT;
using namespace std;
/*static bool comp(const vector<float>& vec1, const vector<int>& vec2){
    return vec1[4] < vec2[4];
}*/
bool sortFuncD( const vector<float>& p1,
           const vector<float>& p2 ) {
 return p1[3] < p2[3];
 }
bool sortFuncSFm( const vector<float>& p1,
           const vector<float>& p2 ) {
 return p1[5] < p2[5];
 }

int mirand(int hasta){
	return (int) (hasta * (rand() / (RAND_MAX + 1.0)));
}

void init_time(timeval& start)
{
        gettimeofday(&start,NULL);
}

double get_time(timeval& start)
{
        timeval end;
        gettimeofday(&end,NULL);
        return (1000000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec))/1000000.0;
}

int genera(){
/*Esta función genera un sistema de nodos distribuidos uniformemente en un cuadrado de superficie lado^2.
Los periodos por ahora los toma en 3600, 1800 y 900 con una distribucion del 97%, 1% y 2% respectivamente
devuelve en el vector nodos con el siguiente formato por fila:
id posx posy dist periodo SFmin SFmax cantMsgs canal(sin asignar)
*/  
  for (int i = 0; i < eds; i++) {
				   ;			   
        // Vector to store column elements
     
            ed.push_back(i);
            posx=mirand(lado)-lado/2;
            posy=mirand(lado)-lado/2;
            ed.push_back(posx);
            ed.push_back(posy);
            distancia=sqrt(posx*posx+posy*posy);
            ed.push_back(distancia);
            int per=mirand(1000);
            if (per<=10){
         	 periodo=1800;
		}
	    else if (per>10 && per<=30){
               periodo=900;
            	}	
            else if (per>30){
              	periodo=3600;
            	}	
	    ed.push_back(periodo);
            ed.push_back(0);
            ed.push_back(0);
            ed.push_back(mcm/periodo); 
            ed.push_back(0);		           
	  //  num += 5;
 
        // Pushing back above 1D vector
        // to create the 2D vector
        nodos.push_back(ed);
        cout<<ed[0]<<" "<<ed[1]<<" "<<ed[2]<<" "<<ed[3]<<" "<<ed[4]<<endl;
        ed.clear();
    }
  return 0;
}

int gateways(int lado){
/*Empiezo con una distribución mia. Tomo 250 m de las esquinas y las distancias entre los centros de 500 m

*/
gw.push_back(mirand(lado));//posx);
gw.push_back(mirand(lado));
GWS.push_back(gw);
gw.clear();
gw.push_back(mirand(lado));;
gw.push_back(mirand(lado));
GWS.push_back(gw);
gw.clear();
gw.push_back(mirand(lado));;
gw.push_back(mirand(lado));
GWS.push_back(gw);
gw.clear();
gw.push_back(mirand(lado));;
gw.push_back(mirand(lado));
GWS.push_back(gw);
gw.clear();

return (0);
}
int asigna(){
/*************************
Esta funcion asigna de manera golosa considerando la distancia de los nodos al gw al cual está asignando end-devices.
Los ordena por distancia al gw y luego en base a la distancia y el periodo del nodo calcula el minimo y maximo SF que puede usar.
Una vez que calculó todos estos valores ordena por SF. Asignando primero los msgs que seran transmitidos con SF7, luego cuando llega al limite posible, pasa al SF8 y asi hasta llegar al 12. Si en el 12 quedan todavia msgs/nodos sin asignar se los pasa al vector NFnodes (non feasible nodes) para que trata de asignarlos en otro gw que tiene que elegir.
*/   
   
   int maxk,i;
   		
   for (i=0;i < nodos.size(); i++){
      if (nodos[i][4]/100>32){
	nodos[i][6]=12;
//	cout<<nodos[i][6]<<endl;
	}
      else if (nodos[i][4]/100>16)
	nodos[i][6]=11;
      else if (nodos[i][4]/100>8){
	nodos[i][6]=10;
//	cout<<nodos[i][6]<<endl;
	}
      else if (nodos[i][4]/100>4)
	nodos[i][6]=9;
      else if (nodos[i][4]/100>2)
	nodos[i][6]=8;
      else if (nodos[i][4]/100>1)
	nodos[i][6]=7;
   }
   for (i=0; i < nodos.size(); i++){
   	maxk=nodos[i][6];
        if (nodos[i][3]<62.5){
        	if (maxk>7)
	 		 nodos[i][5]=7;
	 	else
	 		 nodos[i][5]=maxk;//std::min(7,maxk);
	}
	else if (nodos[i][3]<125)//% && nodos(i,5)==9000)    
          if (maxk>8)
	 		 nodos[i][5]=8;//nodos[i][6]=std::min(8.0,maxk);
	 	else
	 		 nodos[i][5]=maxk;//std::min(7,maxk);

        else if (nodos[i][3]<250)//% && nodos(i,5)==9000)    
          if (maxk>9)
	 		 nodos[i][5]=9;//nodos[i][6]=std::min(9.0,maxk);
	 	else
	 		 nodos[i][5]=maxk;//std::min(7,maxk);

        else if (nodos[i][3]<500)//% && nodos(i,5)==9000)    
          //nodos[i][6]=std::min(10.0,maxk);
          if (maxk>10)
	 		 nodos[i][5]=10;
	 	else
	 		 nodos[i][5]=maxk;//std::min(7,maxk);

        else if (nodos[i][3]<1000)//% && nodos(i,5)==9000)    
          //nodos[i][6]=std::min(11.0,maxk);
          if (maxk>11)
	 		 nodos[i][5]=11;
	 	else
	 		 nodos[i][5]=maxk;//std::min(7,maxk);

        else if (nodos[i][3]<2000)//% && nodos(i,5)==9000)    
          //nodos[i][6]=std::min(12.0,maxk);
          if (maxk>12)
	 		 nodos[i][5]=12;
	 	else
	 		 nodos[i][5]=maxk;//std::min(7,maxk);

   }

    devices=nodos;
    sort(nodos.begin(), nodos.end(), sortFuncSFm);
    MsgToT=0;
    for (int i=0;i++;i<nodos.size())
    	MsgToT+=nodos[i][8];
// como se hará con este template? accumulate(nodos.begin(), a.end(), 0)   
  // Fnodes=nodos;	
  
   int j=0;
   int cont7=0;
   int cont8=0;
   int cont9=0;
   int cont10=0;
   int cont11=0;
   int cont12=0;
   for (i=0;i < nodos.size(); i++){
   	if (nodos[i][5]==7){    // && cont7<3600){
   		Msgs7.push_back(nodos[i]);
   		cont7+=nodos[i][7];
   		if (cont7>3598){
   			for(j=i+1;j<nodos.size();j++){
   				if (nodos[j][5]==7)
   					nodos[j][5]=8;
   				else
   					break;	
   			}
   		}
   	}
   	else if (nodos[i][5]==8){// && cont8<1800){
   		Msgs8.push_back(nodos[i]);
   		cont8+=nodos[i][7];
		if (cont8>1798){
   			j=i+1;
   			for(j=i+1;j<nodos.size();j++){
   				if (nodos[j][5]==8)
   					nodos[j][5]=9;
   				else
   					break;	
   			}
   		}   		
   	} 
   	else if (nodos[i][5]==9){// && cont9<900){
   		Msgs9.push_back(nodos[i]);
   		cont9+=nodos[i][7];
		if (cont9>898){
   			j=i+1;
   			for(j=i+1;j<nodos.size();j++){
   				if (nodos[j][5]==9)
   					nodos[j][5]=10;
   				else
   					break;	
   			}
   		}   		
   	} 
   	else if (nodos[i][5]==10){// && cont10<450){
   		Msgs10.push_back(nodos[i]);
   		cont10+=nodos[i][7];
		if (cont10>448){
   			j=i+1;
   			for(j=i+1;j<nodos.size();j++){
   				if (nodos[j][5]==10)
   					nodos[j][5]=11;
   				else
   					break;	
   			}
   		}   		
   	} 
   	else if (nodos[i][5]==11){// && cont11<225){
   		Msgs11.push_back(nodos[i]);
   		cont11+=nodos[i][7];
		if (cont11>223){
   			j=i+1;
   			for(j=i+1;j<nodos.size();j++){
   				if (nodos[j][5]==11)
   					nodos[j][5]=12;
   				else{
   					break;
   					}	
   			}
   		}   		
   	} 
   	else if (nodos[i][5]==12){// && cont12<112){
   		Msgs12.push_back(nodos[i]);
   		cont12+=nodos[i][7];
		if (cont12>110){
   			j=i+1;
   			for(j=i+1;j<nodos.size();j++){
   				if (nodos[j][5]==12){
   					nodos[j][5]=100;
   					}
   				else
   					break;	
   			}
   		}   		
   	} 
   	else if (nodos[i][5]==100)
   		NFnodes.push_back(nodos[i]);
   }
   return(0);
}

int main(){
/*luego le paso los parametros para que se puedan armar corridas
La idea seria que coloca un conjunto de gws y va asignando de forma golosa a los diferentes gw. Seguramente no queden todos los gws con una cantidad uniforme de end-devices.
Si la idea es minimizar gws entonces esta bien. Se pueden agregar otros criterios como minimizar energia (conviene uno a pleno o dos semiplenos?, seguridad? se cae uno se cae la red completa. Otros...
*/
  /*genero sistema a asignar*/
  int final=0;
  cout<<"Genera sistema"<<endl;
  genera();
  gateways(lado);
  cout<<"guarda nodos generados en nodosOR"<<endl;
  nodosOR=nodos;
  /*TODO hacer un algoritmo de distribución de gws */

 
/* Elejimos un gw */
 for (int k=0;k<nodos.size();k++){
   	nodos[k][1]=(nodos[k][1]+lado/2);
   	nodos[k][2]=(nodos[k][2]+lado/2);
   	nodos[k][3]=sqrt(nodos[k][1]*nodos[k][1]+nodos[k][2]*nodos[k][2]);
   }
for (int h=0;h<GWS.size();h++){
	cout<<"Inicia ciclo de asignación en gws h= "<<h<<endl;
	cout<< "GW "<<h<<" "<<GWS[h][0]<<" "<<GWS[h][1]<<endl;	
	/*cambio las coordenadas a la posicion del gw actual en consideración*/
   	for (int k=0;k<nodos.size();k++){
   		nodos[k][1]=nodos[k][1]-GWS[h][0];
   		nodos[k][2]=nodos[k][2]-GWS[h][1];
   		nodos[k][3]=sqrt(nodos[k][1]*nodos[k][1]+nodos[k][2]*nodos[k][2]);
   		cout<<nodos[k][0]<<" "<<nodos[k][1]<<" "<<nodos[k][2]<<" "<<nodos[k][3]<<endl;
   	}
  /*ordeno eds por periodo*/
  cout<<"cambio coordenadas"<<endl;
  sort(nodos.begin(), nodos.end(), sortFuncD);
  cout<<"asigna"<<endl;
  asigna();
  Msgs7F.insert(Msgs7F.end(),Msgs7.begin(),Msgs7.end());
  Msgs8F.insert(Msgs8F.end(),Msgs8.begin(),Msgs8.end());
  Msgs9F.insert(Msgs9F.end(),Msgs9.begin(),Msgs9.end());
  Msgs10F.insert(Msgs10F.end(),Msgs10.begin(),Msgs10.end());
  Msgs11F.insert(Msgs11F.end(),Msgs11.begin(),Msgs11.end());
  Msgs12F.insert(Msgs12F.end(),Msgs12.begin(),Msgs12.end());
  nodos.clear();
  if (NFnodes.size()>0){
  	nodos.clear();
  	nodos=NFnodes;
  }
  else
  	final=1;	 	
   
    cout<<"SF7 "<<Msgs7.size()<<"     "<<Msgs7F.size()<<endl;
    cout<<"SF8 "<<Msgs8.size()<<"     "<<Msgs8F.size()<<endl;
    cout<<"SF9 "<<Msgs9.size()<<"     "<<Msgs9F.size()<<endl;
    cout<<"SF10 "<<Msgs10.size()<<"     "<<Msgs10F.size()<<endl;
    cout<<"SF11 "<<Msgs11.size()<<"     "<<Msgs11F.size()<<endl;
    cout<<"SF12 "<<Msgs12.size()<<"     "<<Msgs12F.size()<<endl;
    cout<<"NF "<<NFnodes.size()<<endl;
    Msgs7.clear();
    Msgs8.clear();
    Msgs9.clear();
    Msgs10.clear();
    Msgs11.clear();
    Msgs12.clear();
    NFnodes.clear();
    if (final==1){
    	final=0;
    	break;	
    }	
       for (int k=0;k<nodos.size();k++){
   	nodos[k][1]=nodos[k][1]+GWS[h][0];
   	nodos[k][2]=nodos[k][2]+GWS[h][1];
   }

}
  return 0;
}

